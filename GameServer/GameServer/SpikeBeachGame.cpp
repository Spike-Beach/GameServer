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
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	_gameStatus = GameStatus::EMPTY;
	for (size_t i = 0; i < 4 ; i++)
	{
		if (_users[i].second != nullptr)
		{
			g_SBUserManager.ReleaseUser(_users[i].second);
		}
		_users[i].first = -1;
		_users[i].second = nullptr;
		_users[i].second->clear();
	}
	_ball.Reset();
	_redScore = 0;
	_blueScore = 0;
	_gameId = -1;
	_leaveUserIdx = -1;
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::Clear", std::to_string(_gameId) + " Game is cleared");
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

std::vector<char> SpikeBeachGame::GetSerialiedSyncPacket()
{
	std::shared_lock<std::shared_mutex> sharedLock(_syncPacketMutex);
	if (_serializedSyncPacket.empty() || _lastSyncTime > _packetGenTime)
	{
		sharedLock.unlock();
		GenSerializedSyncPacket();
	}
	return _serializedSyncPacket;
}

bool SpikeBeachGame::SetGame(INT32 gameId, Team redTeam, Team blueTeam)
{
	if (_gameStatus.load() != GameStatus::EMPTY)
	{
		return false;
	}

	std::unique_lock<std::shared_mutex> lock(_gameMutex);
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

bool SpikeBeachGame::UserIn(SBUser* user)
{
	if (_gameStatus != GameStatus::WAITING)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", std::to_string(_gameId) + " Game is not waiting, user : " + std::to_string(user->GetId()));
		return false;
	}

	std::unique_lock<std::shared_mutex> lock(_gameMutex);
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
			return true;
		}
	}

	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "Invalid game enter. user : " + std::to_string(user->GetId()));
	return false;
}

bool SpikeBeachGame::UserOut(SBUser* user)
{
	if (_gameStatus == GameStatus::EMPTY)
	{
		return false;
	}

	std::unique_lock<std::shared_mutex> lock(_gameMutex);
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

bool SpikeBeachGame::Controll(INT64 userId, INT64 ctlTime, Acceleration acc)
{
	SBUser* userPtr = nullptr;
	INT16 userIdx = FindUser(userId, &userPtr);
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	if (userIdx == -1 || userPtr == nullptr)
	{
		return false;
	}
	if (_roundStartTime > now)
	{
		return false;
	}
	// https://stackoverflow.com/questions/31255486/how-do-i-convert-a-stdchronotime-point-to-long-and-back
	// 1970년 1월 1일 00:00:00 UTC (UNIX epoch)부터의 시간. 약 292,271 년 후 오버플로우 발생.
	std::chrono::milliseconds dur(ctlTime);
	std::chrono::system_clock::time_point ctlTimePoint(dur);
	userPtr->Controll(ctlTimePoint, acc);
	ControllNtf ntf;
	ntf.userIdx = userIdx;
	ntf.controllTime = ctlTime;
	ntf.contollAcc = acc;
	NoticeInGame(ntf.Serialize());
	return true;
}

bool SpikeBeachGame::UpdateLatency(INT64 userId, INT64 clientTime)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].first == userId)
		{
			_users[i].second->UpdateLatency(clientTime);
			return true;
		}
	}
	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::SetLaytency", "Invalid userId user : " + std::to_string(userId));
	return false;
}

// true : score / false : no score
bool SpikeBeachGame::PlayingSync()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	SyncResult result = _ball.Sync(now);
	if (result != SyncResult::NONE)
	{
		Score(result);
		return true;
	}

	for (size_t idx = 0; idx < 4; idx++)
	{
		if (_users[idx].second == nullptr)
		{
			g_logger.Log(LogLevel::ERR, "SpikeBeachGame::PlayingSync", "User is nullptr. user : " + std::to_string(_users[idx].first));
			return false;
		}
		_users[idx].second->Sync(now);
	}

	_lastSyncTime = std::chrono::system_clock::now();
	return false;
}

// 대기 시간이 지나면 유저들에게 통보 후 세션 해제 대기
// 대기 시간이 지나지 않았고 모든 인원이 모였으면 게임 시작 신호 및 상태 변경
bool SpikeBeachGame::WaitUserSync()
{
	if (std::chrono::system_clock::now() > _waitDeadLine)
	{
		std::unique_lock<std::shared_mutex> lock(_gameMutex);
		GameTimeoutNtf timeoutPacket;
		NoticeInGame(timeoutPacket.Serialize());
		         
		for (size_t i = 0; i < 4; i++)
		{
			if (_users[i].second != nullptr)
			{
				g_sessionManager.ReleaseSession(_users[i].second->GetSessionId(), false);
			}
		}
		return false;
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (_users[i].second == nullptr)
		{
			return true;
		}
	}

	_gameStatus = GameStatus::PLAYING;
	_roundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(ROUND_COUNT_DOWN_SEC);
	
	GameStartNtf startNtf;
	// _roundStartTime를 INT64로 변환하여 startNtf.gameStartTime에 저장
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(_roundStartTime.time_since_epoch());
	startNtf.gameStartTime = ms.count();
	for (size_t i = 0; i < startNtf.nickNames.size(); i++)
	{
		startNtf.nickNames[i] = _users[i].second->GetNickName();
	}
	NoticeInGame(startNtf.Serialize());
	ResetToNewGame();

	g_logger.Log(LogLevel::INFO, "WaitUserSync", std::to_string(_gameId) + "Game Start");
	return true;
}

bool SpikeBeachGame::LeaveSync
()
{
	if (_leaveUserIdx < 0)
	{
		return true;
	}
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	if (_leaveUserIdx < 2)
	{
		RedWin();
	}
	else
	{
		BlueWin();
	}
	return true;
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

bool SpikeBeachGame::Score(SyncResult scoreResult)
{
	if (scoreResult == SyncResult::RED_SCORE)
	{ 
		_redScore++; 
		if (_redScore >= 3) // TODO
		{ 
			RedWin();
			return true;
		}
	}
	else if (scoreResult == SyncResult::BLUE_SCORE)
	{ 
		_blueScore++; 
		if(_blueScore >= 3) // TODO
		{ 
			BlueWin(); 
			return true;
		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		_users[i].second->Reset();
	}
	_ball.Reset();
	_roundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(ROUND_COUNT_DOWN_SEC);
	// TODO : 결과 패킷 공지
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::PlayingSync", std::to_string(_gameId) + " Game score is");
	return true;
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

std::vector<char> SpikeBeachGame::GenSerializedSyncPacket()
{
	SyncRes syncRes;

	std::unique_lock<std::shared_mutex> uniqueLock(_syncPacketMutex);
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	syncRes.syncTime = ms.count();
	for (size_t i = 0; i < 4; i++)
	{
		syncRes.latency[i] = _users[i].second->GetLatency();
		syncRes.users[i] = _users[i].second->GetMotionData();
	}
	_packetGenTime = _lastSyncTime;
	_serializedSyncPacket = syncRes.Serialize();
	return _serializedSyncPacket;
}