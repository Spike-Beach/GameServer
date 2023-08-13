#include "pch.h"
#include "SBManager.h"
#include "TaskManager.h"
#include "GameSetTask.h"
#include "GameSyncTask.h"
#include "ParsedRoomInfo.h"
#include "SBUserManager.h"

SBManager::SBManager()
	: _isRunning(true)
{
	Init();
}
SBManager::~SBManager() {}

bool SBManager::Init()
{
	_poolSize = g_config.config["GameSettings"]["GameCount"].asInt();
	_gamePool.resize(_poolSize);
	for (size_t i = 0; i < _poolSize; i++)
	{
		_emptyGames.push(&_gamePool[i]);
	}
	return true;
}

bool SBManager::SetGame(std::string infoStr)
{
	ParsedRoomInfo info;
	try
	{
		if (info.Parse(infoStr) == false)
		{
			g_logger.Log(LogLevel::CRITICAL, "SBManager::SetGame", "ParsedRoomInfo::Parse failed");
			return false;
		}
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::CRITICAL, "SBManager::SetGame", "ParsedRoomInfo::Parse Exception, " + std::string(e.what()));
		return false;
	}
	
	std::unique_lock<std::mutex> lock(_runningGameMutex);
	if (_emptyGames.empty() == false)
	{
		SpikeBeachGame* emptyGame = _emptyGames.top();
		if (emptyGame->SetGame(info.roomId, info.redTeam, info.blueTeam) == false)
		{
			return false;
		}
		_emptyGames.pop();
		_runningGames.insert(std::make_pair(info.roomId, emptyGame));
	}
}

// content�ܿ��� ���� ���ҽ� ó��.
bool SBManager::UserEnterGame(INT32 roomId, SBUser* user)
{
	std::unique_lock<std::mutex> lock(_runningGameMutex);
	auto iter = _runningGames.find(roomId);
	if (iter != _runningGames.end())
	{
		return iter->second->UserIn(user);
	}
	g_logger.Log(LogLevel::ERR, "SBManager::UserEnterGame", "Not found roomId : " + std::to_string(roomId));
	return false;
}

// ���� ��ũ. 
//   ������� ���� : �����ο� ��ũ �� Ÿ�ӿ����� ���� �ʱ�ȭ �� �뺸
void SBManager::SyncGames()
{
	GameStatus status;
	for (auto runningGame : _runningGames)
	{
		status = runningGame.second->Status();
		if (status == GameStatus::WAITING)
		{
			if (runningGame.second->WaitUserSync() == false)
			{
				runningGame.second->Clear();
				_runningGames.erase(runningGame.first);
				_emptyGames.push(runningGame.second);
			}
		}
		else if (status == GameStatus::PLAYING)
		{
			runningGame.second->PlayingSync();
		}
	}
}

bool SBManager::IsRuning()
{
	return _isRunning.load();
}

void SBManager::SetGameResult(const GameResult& result)
{
	std::unique_lock<std::shared_mutex> lock(_gameResultMutex);
	_gameResultWaitingQueue.push(result);
}

std::optional<GameResult> SBManager::GetGameResult()
{
	std::shared_lock<std::shared_mutex> sharedLock(_gameResultMutex);
	if (_gameResultWaitingQueue.empty() == false)
	{
		sharedLock.unlock();

		std::unique_lock<std::shared_mutex> uniqueLock(_gameResultMutex);
		GameResult result = _gameResultWaitingQueue.front();
		_gameResultWaitingQueue.pop();
		return result;
	}
	return std::nullopt;
}
