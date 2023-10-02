#include "pch.h"
#include "SpikeBeachGame.h"
#include "SBUserManager.h"
#include "SBManager.h"
#include "SessionManager.h"
#include "SBUtils.h"
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"
#include "Disconn.h"
#include "GameTimeoutNtf.h"
#include "GameStartNtf.h"
#include "Controll.h"
#include "Sync.h"

std::vector<char> SpikeBeachGame::GetSerializedSyncPacket(INT64 userId)
{
	SyncRes syncRes;
	std::shared_lock<std::shared_mutex> sharedLock(_gameMutex);
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second->GetId() == userId)
		{
			syncRes.syncReqTime = _users[i].second->GetLastSyncTime();
		}
		syncRes.ttses[i] = _users[i].second->GetTTS();
		syncRes.users[i] = _users[i].second->GetMotionData();
	}
	return syncRes.Serialize();
}

bool SpikeBeachGame::SetGame(INT32 gameId, Team redTeam, Team blueTeam)
{
	if (_gameStatus.load() != GameStatus::EMPTY)
	{
		g_logger.Log(LogLevel::CRITICAL, "SpikeBeachGame::SetGame", std::to_string(_gameId) + " Game is Not Empty");
		return false;
	}

	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	_gameId = gameId;
	_gameStatus = GameStatus::WAITING;
	_waitDeadLine = std::chrono::system_clock::now() + std::chrono::seconds(WAIT_SEC);
	
	_users[0] = std::make_pair(redTeam[0].id, nullptr);
	_users[1] = std::make_pair(redTeam[1].id, nullptr);
	_users[2] = std::make_pair(blueTeam[0].id, nullptr);
	_users[3] = std::make_pair(blueTeam[1].id, nullptr);

	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::SetGame", std::to_string(_gameId) + " Game is set");
	return true;
}

INT16 SpikeBeachGame::UserIn(SBUser* user)
{
	if (_gameStatus != GameStatus::WAITING)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", std::to_string(_gameId) + " Game is not waiting, user : " + std::to_string(user->GetId()));
		return -1;
	}

	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	INT64 userId = user->GetId();

	for (size_t idx = 0; idx < 4; idx++)
	{
		if (_users[idx].first == userId)
		{
			if (_users[idx].second != nullptr)
			{
				g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "User already in game. user : " + std::to_string(user->GetId()));
				return false;
			}
			_users[idx].second = user;
			g_logger.Log(LogLevel::INFO, "SpikeBeachGame::UserIn", "User in " + std::to_string(_gameId) + " game. user : " + std::to_string(_users[idx].second->GetId()));
			return idx;
		}
	}

	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "Invalid game enter. user : " + std::to_string(user->GetId()));
	return -1;
}

bool SpikeBeachGame::UserOut(SBUser* user)
{
	if (_gameStatus == GameStatus::EMPTY)
	{
		return false;
	}

	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second != NULL
			&& _users[i].second->GetId() == user->GetId())
		{
			_leaveUserIdx = i;
			_users[i].second = nullptr;
			g_SBUserManager.ReleaseUser(user);
			if (_gameStatus == GameStatus::PLAYING)
			{
				_gameStatus = GameStatus::SOMEONE_LEANVE;
			}
			return true;
		}
	}

	g_logger.Log(LogLevel::CRITICAL, "SpikeBeachGame::UserOut", "Invalid game enter. user : " + std::to_string(user->GetId()));
	return false;
}

bool SpikeBeachGame::Controll(INT64 userId, float xCtl, float yCtl)
{
	SBUser* userPtr = nullptr;
	INT16 userIdx = FindUser(userId, &userPtr);
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	if (userIdx == -1 || userPtr == nullptr)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachGame::Controll", "Invalid user : " + std::to_string(userId));
		return false;
	}
	if (now < _roundStartTime)
	{
		return false;
	}

	INT64 delayInt64 = CalControllDelay(userId);
	std::chrono::milliseconds delay(delayInt64);
	std::pair<float, float> dir(xCtl, yCtl);
	if (userIdx <= 1) // red팀은 바라보는 방향이 반대이므로 x축 방향을 반대로.
	{
		dir.first *= -1;
	}
	if (userPtr->Controll(now + delay, dir) == true)
	{
		ControllNtf ntf;
		ntf.userIdx = userIdx;
		ntf.expectedSyncDuration = delayInt64;
		ntf.xAppliedCtl = dir.first;
		ntf.yAppliedCtl = dir.second;
		NoticeInGame(ntf.Serialize());
		return true;
	}
	return false;
}

INT64 SpikeBeachGame::CalControllDelay(INT64 sendUserId)
{
	INT64 calTTS = 0;
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second != NULL && _users[i].second->GetId() != sendUserId)
		{
			// Todo: 완성본에서 튀는 tts걸러주는 로직 추가.
			INT64 tts = _users[i].second->GetTTS();
			calTTS += tts / 6;
		}
	}
	return calTTS;
}

bool SpikeBeachGame::SetUserTimes(INT64 userId, INT64 clientTime, INT64 tts)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].first == userId)
		{
			_users[i].second->SetUserTimes(clientTime, tts);
			return true;
		}
	}
	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::SetLaytency", "Invalid userId user : " + std::to_string(userId));
	return false;
}

SyncResult SpikeBeachGame::PlayingSync()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	BallResult result = _ball.Sync(now);
	if (result != BallResult::NONE)
	{
		return Score(result);
	}

	for (size_t idx = 0; idx < 4; idx++)
	{
		if (_users[idx].second == nullptr)
		{
			g_logger.Log(LogLevel::ERR, "SpikeBeachGame::PlayingSync", "User is nullptr. user : " + std::to_string(_users[idx].first));
			return SyncResult::ERR;
		}
		_users[idx].second->Sync(now);
	}

	return SyncResult::NONE;
}

SyncResult SpikeBeachGame::WaitUserSync()
{
	if (std::chrono::system_clock::now() > _waitDeadLine)
	{
		std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
		GameTimeoutNtf timeoutPacket;
		NoticeInGame(timeoutPacket.Serialize());
		         
		for (size_t i = 0; i < 4; i++)
		{
			if (_users[i].second != nullptr)
			{
				g_sessionManager.ReleaseSession(_users[i].second->GetSessionId(), false);
			}
		}
		return SyncResult::TIMEOVER;
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second == nullptr)
		{
			return SyncResult::NONE;
		}
	}

	_gameStatus = GameStatus::PLAYING;
	_roundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(ROUND_COUNT_DOWN_SEC);
	
	GameStartNtf startNtf;
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(_roundStartTime.time_since_epoch());
	startNtf.gameStartTime = ms.count();
	for (size_t i = 0; i < startNtf.nickNames.size(); i++)
	{
		startNtf.nickNames[i] = _users[i].second->GetNickName();
	}
	NoticeInGame(startNtf.Serialize());
	ResetToNewGame();

	g_logger.Log(LogLevel::INFO, "WaitUserSync", std::to_string(_gameId) + "Game Start");
	return SyncResult::NONE;
}

SyncResult SpikeBeachGame::LeaveSync()
{
	if (_leaveUserIdx < 0)
	{
		return SyncResult::NONE;
	}
	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	if (_leaveUserIdx < 2)
	{
		RedWin();
	}
	else
	{
		BlueWin();
	}
	return SyncResult::SOMEONELEAVE;
}

void SpikeBeachGame::NoticeInGame(std::vector<char>&& notify)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second != nullptr)
		{
			g_sessionManager.SendData(_users[i].second->GetSessionId(), notify);
		}
	}
}

SyncResult SpikeBeachGame::Score(BallResult ballResult)
{
	if (ballResult == BallResult::SCORE_RED)
	{ 
		_redScore++; 
		if (_redScore >= 3) // TODO
		{ 
			RedWin();
			return SyncResult::GAMEFIN;
		}
	}
	else if (ballResult == BallResult::SCORE_BLUE)
	{ 
		_blueScore++; 
		if(_blueScore >= 3) // TODO
		{ 
			BlueWin(); 
			return SyncResult::GAMEFIN;
		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		_users[i].second->Reset();
	}
	_ball.Reset();
	_roundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(ROUND_COUNT_DOWN_SEC);
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::PlayingSync", std::to_string(_gameId) + " Game score is");
	return SyncResult::NONE;
}

void SpikeBeachGame::RedWin()
{
	GameResult result;
	_gameStatus = GameStatus::FINISHING;

	size_t i = 0;
	for (; i < 2; i++)
	{
		result.winner[i].id = _users[i].first;
	}
	for (; i < 4; i++)
	{
		result.loser[i - 2].id = _users[i].first;
	}
	result.startTime = _gameStartTime;
	result.finishTime = std::chrono::system_clock::now();
	g_SBManager.SetGameResult(result);
	// TODO: 결과 통지
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::RedWin()", std::to_string(_gameId) + "Game score is:");
}

void SpikeBeachGame::BlueWin()
{
	GameResult result;
	_gameStatus = GameStatus::FINISHING;

	size_t i = 0;
	for (; i < 2; i++)
	{
		result.winner[i].id = _users[i + 2].first;
	}
	for (; i < 4; i++)
	{
		result.loser[i - 2].id = _users[i - 2].first;
	}
	result.startTime = _gameStartTime;
	result.finishTime = std::chrono::system_clock::now();
	g_SBManager.SetGameResult(result);
	// TODO: 결과 통지
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::BlueWin()", std::to_string(_gameId) + "Game score is:");
}

INT16 SpikeBeachGame::FindUser(INT64 userId, SBUser** userPtr)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].first == userId)
		{
			*userPtr = _users[i].second;
			return i;
		}
	}
	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "Invalid game enter. user : " + std::to_string(userId));
	*userPtr = nullptr;
	return -1;
}

SpikeBeachGame::SpikeBeachGame()
	:_gameStatus(GameStatus::EMPTY), _gameId(-1), _redScore(0), _blueScore(0), _leaveUserIdx(-1)
{
	for (size_t i = 0; i < 4; i++)
	{
		_users[i].first = -1;
		_users[i].second = nullptr;
	}
}

GameStatus SpikeBeachGame::GetStatus()
{
	return _gameStatus;
}

void SpikeBeachGame::Clear()
{
	std::unique_lock<std::shared_mutex> uniqueLock(_gameMutex);
	_gameStatus = GameStatus::EMPTY;
	for (size_t i = 0; i < 4 ; i++)
	{
		if (_users[i].second != nullptr)
		{
			g_SBUserManager.ReleaseUser(_users[i].second);
			_users[i].second->clear();
			_users[i].second = nullptr;
		}
		_users[i].first = -1;
	}
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::Clear", std::to_string(_gameId) + " Game is cleared");
	_ball.Reset();
	_redScore = 0;
	_blueScore = 0;
	_gameId = -1;
	_leaveUserIdx = -1;
}

/*
	r1 (1860.006355, 292.065717, 20)
	r2 (2112.010027, 269.346001, 20)

	b1 (1860.005797, 1350.856280, 20)
	b2 (2112.010468, 1337.891186, 20)
*/
void SpikeBeachGame::ResetToNewGame()
{
	_ball.Reset();
	_users[0].second->Reset(1860.006355, 292.065717, 20);
	_users[1].second->Reset(2112.010027, 269.346001, 20);
	_users[2].second->Reset(1860.005797, 1350.856280, 20);
	_users[3].second->Reset(2112.010468, 1337.891186, 20);
}