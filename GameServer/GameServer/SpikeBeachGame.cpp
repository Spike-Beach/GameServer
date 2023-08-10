#include "pch.h"
#include "SpikeBeachGame.h"
#include "SessionManager.h"
#include "SBUserManager.h"
#include "CloseConnPacket.h"

SpikeBeachGame::SpikeBeachGame()
:_gameStatus(GameStatus::EMPTY)
{

}

GameStatus SpikeBeachGame::Status()
{
	return _gameStatus;
}

void SpikeBeachGame::Clear()
{
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
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::Clear", std::to_string(_gameId) + " Game is cleared");
	_gameId = -1;
}

bool SpikeBeachGame::SetGame(INT32 gameId, Team redTeam, Team blueTeam)
{
	if (_gameStatus != GameStatus::EMPTY)
	{
		return false;
	}
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
	g_logger.Log(LogLevel::INFO, "SpikeBeachGame::UserIn", "User in " + std::to_string(_gameId) + " game. user : " + std::to_string(user->GetId()));
	return true;
}

//bool Game::UserOut(INT32 sessionId)
//{
//	if (_gameStatus == GameStatus::PLAYING)
//	{
//		return false;
//	}
//	_session2UserMap.erase(sessionId);
//	return true;
//}

// true : score / false : no score
bool SpikeBeachGame::PlayingSync()
{
	SyncResult result = _ball.Sync();
	if (result != SyncResult::NONE)
	{
		if (result == SyncResult::RED_WIN)
		{
			_redScore++;
			// 점수 조건문
		}
		else if (result == SyncResult::BLUE_WIN)
		{
			_blueScore++;
		}
		for (auto redUser : _redTeam)
		{
			redUser.second->Reset();
		}
		for (auto blueUser : _blueTeam)
		{
			blueUser.second->Reset();
		}
		// TODO : 결과 패킷 공지
		g_logger.Log(LogLevel::INFO, "SpikeBeachGame::PlayingSync", std::to_string(_gameId) + " Game score is");
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
	
	std::vector<char> syncData = GenGameSyncData();
	NoticeInGame(syncData);
	return false;
}

// 대기 시간이 지나면 유저들에게 통보 후 세션 해제 대기
// 대기 시간이 지나지 않았고 모든 인원이 모였으면 게임 시작 신호 및 상태 변경
bool SpikeBeachGame::WaitUserSync()
{
	if (std::chrono::system_clock::now() > _waitDeadLine)
	{
		CloseConnPacket packet;
		std::vector<char> disConnNtf = packet.Serialize();
		NoticeInGame(disConnNtf);
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
	// TODO: send notice
	return true;
}

void SpikeBeachGame::NoticeInGame(std::vector<char>& notify)
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

std::vector<char> SpikeBeachGame::GenGameSyncData()
{
	// TODO
	return std::vector<char>();
}