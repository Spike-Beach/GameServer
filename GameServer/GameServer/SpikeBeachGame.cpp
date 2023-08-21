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
:_gameStatus(GameStatus::EMPTY)
{
	_gameId = -1;
	_leaveUser.first = TeamKind::EMPTY;
	_leaveUser.second = nullptr;
}

GameStatus SpikeBeachGame::GetStatus()
{
	return _gameStatus;
}

void SpikeBeachGame::Clear()
{
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	_gameStatus = GameStatus::EMPTY;
	for (auto redUser : _redTeam)
	{
		if (redUser.second != nullptr)
		{
			g_SBUserManager.ReleaseUser(redUser.second);
		}
	}
	for (auto blueUser : _blueTeam)
	{
		if (blueUser.second != nullptr)
		{
			g_SBUserManager.ReleaseUser(blueUser.second);
		}
	}
	_redTeam.clear();
	_blueTeam.clear();
	_ball.Reset();
	_redScore = 0;
	_blueScore = 0;
	_gameId = -1;
	_leaveUser.first = TeamKind::EMPTY;
	_leaveUser.second = nullptr;
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
	_redTeam[0]->Reset(1860.006355, 292.065717, 20);
	_redTeam[1]->Reset(2112.010027, 269.346001, 20);
	_blueTeam[0]->Reset(1860.005797, 1350.856280, 20);
	_blueTeam[1]->Reset(2112.010468, 1337.891186, 20);
}

std::vector<char> SpikeBeachGame::GetSerialiedSyncPacket()
{
	std::shared_lock<std::shared_mutex> sharedLock(_syncPacketMutex);
	if (_lastSyncTime > _packetGenTime)
	{
		sharedLock.unlock();
		return GenSerializedSyncPacket();
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
	
	_redTeam.insert(std::make_pair(redTeam[0].id, nullptr));
	_redTeam.insert(std::make_pair(redTeam[1].id, nullptr));
	
	_blueTeam.insert(std::make_pair(blueTeam[0].id, nullptr));
	_blueTeam.insert(std::make_pair(blueTeam[1].id, nullptr));

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
	auto iter = _redTeam.find(userId);
	if (iter == _redTeam.end())
	{
		iter = _blueTeam.find(userId);
		if (iter == _blueTeam.end())
		{
			g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "Invalid game enter. user : " + std::to_string(user->GetId()));
			return false;
		}
	}
	if (iter->second != nullptr)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "User already in game. user : " + std::to_string(user->GetId()));
		return false;
	}

	iter->second = user;
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::UserIn", "User in " + std::to_string(_gameId) + " game. user : " + std::to_string(iter->second->GetId()));
	return true;
}

bool SpikeBeachGame::UserOut(SBUser* user)
{
	if (_gameStatus == GameStatus::EMPTY)
	{
		return false;
	}

	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	auto iter = _redTeam.find(user->GetId());
	_leaveUser.first = TeamKind::RED;
	if (iter == _redTeam.end())
	{
		iter = _blueTeam.find(user->GetId());
		_leaveUser.first = TeamKind::BLUE;
		if (iter == _blueTeam.end())
		{
			g_logger.Log(LogLevel::CRITICAL, "SpikeBeachGame::UserOut", "Invalid game enter. user : " + std::to_string(user->GetId()));
			return false;
		}
	}
	_leaveUser.second = iter->second;

	if (_gameStatus == GameStatus::PLAYING)
	{
		_gameStatus = GameStatus::SOMEONE_LEANVE;
	}
	iter->second = nullptr;
	g_SBUserManager.ReleaseUser(user);
	return true;
}

bool SpikeBeachGame::Controll(INT64 userId, INT64 ctlTime, Acceleration acc, Acceleration stopAcc)
{
	SBUser* userPtr = nullptr;
	INT16 userIdx = FindUser(userId, &userPtr);
	if (userIdx == -1 || userPtr == nullptr)
	{
		return false;
	}

	// INT64 to std::chrono::system_clock::time_point
	std::chrono::system_clock::time_point ctlTimePoint = std::chrono::system_clock::from_time_t(ctlTime / 1000);
	userPtr->Controll(ctlTimePoint, acc, stopAcc);
	ControllNtf ntf;
	ntf.userIdx = userIdx;
	ntf.controllTime = ctlTime;
	ntf.contollAcc = acc;
	ntf.stopAcc = stopAcc;
	NoticeInGame(ntf.Serialize()); // TODO move check
	return true;
}

// true : score / false : no score
bool SpikeBeachGame::PlayingSync()
{
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	SyncResult result = _ball.Sync();
	if (result != SyncResult::NONE)
	{
		Score(result);
		return true;
	}

	for (auto redUser : _redTeam)
	{
		redUser.second->Sync();
	}
	for (auto blueUser : _blueTeam)
	{
		blueUser.second->Sync();
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
		for (auto redUser : _redTeam)
		{
			if (redUser.second != nullptr)
			{
				g_sessionManager.ReleaseSession(redUser.second->GetSessionId(), false);
			}
		}
		for (auto blueUser : _blueTeam)
		{
			if (blueUser.second != nullptr)
			{
				g_sessionManager.ReleaseSession(blueUser.second->GetSessionId(), false);
			}
		}
		return false;
	}
	for (auto redUser : _redTeam)
	{
		if (redUser.second == nullptr)
		{
			return true;
		}
	}
	for (auto blueUser : _blueTeam)
	{
		if (blueUser.second == nullptr)
		{
			return true;
		}
	}

	_gameStatus = GameStatus::PLAYING;
	_roundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(ROUND_COUNT_DOWN_SEC);
	
	GameStartNtf startNtf;
	// _roundStartTime를 INT64로 변환하여 startNtf.gameStartTime에 저장
	std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(_roundStartTime.time_since_epoch());
	startNtf.gameStartTime = us.count();
	NoticeInGame(startNtf.Serialize());

	ResetToNewGame();
	g_logger.Log(LogLevel::INFO, "WaitUserSync", std::to_string(_gameId) + "Game Start");
	return true;
}

bool SpikeBeachGame::LeaveSync()
{
	std::unique_lock<std::shared_mutex> lock(_gameMutex);
	if (_leaveUser.first == TeamKind::RED)
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
	// 모든 멤버에세 SyncNotice를 보낸다.
	for (auto redUser : _redTeam)
	{
		if (redUser.second != nullptr)
		{
			g_sessionManager.SendData(redUser.second->GetSessionId(), notify);
		}
	}
	for (auto blueUser : _blueTeam)
	{
		if (blueUser.second != nullptr)
		{
			g_sessionManager.SendData(blueUser.second->GetSessionId(), notify);
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

	for (auto redUser : _redTeam)
	{
		redUser.second->Reset();
	}
	for (auto blueUser : _blueTeam)
	{
		blueUser.second->Reset();
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
	result.winner[0].id = _redTeam.begin()->first;
	result.winner[1].id = (++_redTeam.begin())->first;
	result.loser[0].id = _blueTeam.begin()->first;
	result.loser[1].id = (++_blueTeam.begin())->first;
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
	result.winner[0].id = _blueTeam.begin()->first;
	result.winner[1].id = (++_blueTeam.begin())->first;
	result.loser[0].id = _redTeam.begin()->first;
	result.loser[1].id = (++_redTeam.begin())->first;
	result.startTime = _gameStartTime;
	result.finishTime = std::chrono::system_clock::now();
	g_SBManager.SetGameResult(result);
	// TODO: 결과 통지
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::BlueWin()", std::to_string(_gameId) + "Game score is:");
}

INT16 SpikeBeachGame::FindUser(INT64 userId, SBUser** userPtr)
{
	INT16 idx = 0;
	for (const auto& red : _redTeam)
	{
		if (red.first == userId)
		{
			*userPtr = red.second;
			return idx;
		}
		++idx;
	}
	for (const auto& blue : _blueTeam)
	{
		if (blue.first == userId)
		{
			*userPtr = blue.second;
			return idx;
		}
		++idx;
	}
	g_logger.Log(LogLevel::ERR, "SpikeBeachGame::UserIn", "Invalid game enter. user : " + std::to_string(userId));
	*userPtr = nullptr;
	return -1;
}

std::vector<char> SpikeBeachGame::GenSerializedSyncPacket()
{
	SyncRes syncRes;

	std::unique_lock<std::shared_mutex> uniqueLock(_syncPacketMutex);
	std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	syncRes.milSec = us.count();
	syncRes.delay = {-1, -1, -1, -1}; // TODO
	syncRes.red1 = _redTeam[0]->GetMotionData();
	syncRes.red2 = _redTeam[1]->GetMotionData();
	syncRes.blue1 = _blueTeam[0]->GetMotionData();
	syncRes.blue2 = _blueTeam[1]->GetMotionData();

	_packetGenTime = _lastSyncTime;
	_serializedSyncPacket = syncRes.Serialize();
	return _serializedSyncPacket;
}