#include "pch.h"
#include "SpikeBeachGame.h"
#include "SessionManager.h"
#include "SBUserManager.h"

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

	return true;
}

bool SpikeBeachGame::UserIn(SBUser* user)
{
	if (_gameStatus != GameStatus::WAITING)
	{
		// log
		return false;
	}

	INT64 userId = user->GetId();
	auto iter = _redTeam.find(userId);
	if (iter == _redTeam.end())
	{
		iter = _blueTeam.find(userId);
		if (iter == _blueTeam.end())
		{
			// log
			return false;
		}
	}
	
	if (iter->second != nullptr)
	{
		// log
		return false;
	}
	iter->second = user;
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
			// ���� ���ǹ�
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
		SyncNotice();
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
	
	SyncNotice(); // ������ �� ���� �����ؼ�
	return false;
}

// ��� �ð��� ������ �����鿡�� �뺸 �� ���� ���� ���
// ��� �ð��� ������ �ʾҰ� ��� �ο��� ������ ���� ���� ��ȣ �� ���� ����
bool SpikeBeachGame::WaitUserSync()
{
	if (std::chrono::system_clock::now() > _waitDeadLine)
	{
		for (auto redUser : _redTeam)
		{
			if (redUser.second != nullptr)
			{
				// send notice
				g_sessionManager.ReleaseSession(redUser.second->GetSessionId(), false);
			}
		}
		for (auto blueUser : _blueTeam)
		{
			if (blueUser.second != nullptr)
			{
				// send notice
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

void SpikeBeachGame::SyncNotice()
{
	// ��� ������� SyncNotice�� ������.
}