#include "pch.h"
#include "SBManager.h"
#include "ServerConfigManager.h"
#include "SBUtils.h"
#include "ParsedRoomInfo.h"

SBManager::SBManager()
{
	_isRunning.store(true);
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

INT16 SBManager::UserEnterGame(INT32 roomId, SBUser* user)
{
	auto iter = _runningGames.find(roomId);
	if (iter != _runningGames.end())
	{
		return iter->second->UserIn(user);
	}
	g_logger.Log(LogLevel::ERR, "SBManager::UserEnterGame", "Not found roomId : " + std::to_string(roomId));
	return -1;
}

bool SBManager::UserLeaveGame(INT32 roomId, SBUser* user)
{
	auto iter = _runningGames.find(roomId);
	if (iter != _runningGames.end())
	{
		iter->second->UserOut(user);
		return true;
	}
	g_logger.Log(LogLevel::ERR, "SBManager::UserLeaveGame", "Not found roomId : " + std::to_string(roomId));
	return false;
}

SpikeBeachGame* SBManager::GetGame(INT32 roomId)
{
	if (_runningGames.contains(roomId))
	{
		return _runningGames[roomId];
	}
	g_logger.Log(LogLevel::ERR, "SBManager::GetGame", "Not found roomId : " + std::to_string(roomId));
	return nullptr;
}

// 게임 싱크. 
//   대기중인 게임 : 게임인원 싱크 및 타임오버시 게임 초기화 및 통보
void SBManager::SyncGames()
{
	GameStatus status;
	std::stack<INT32> makeEmptyGameStack;

	for (auto runningGame : _runningGames)
	{
		status = runningGame.second->GetStatus();
		if (status == GameStatus::WAITING)
		{
			if (runningGame.second->WaitUserSync() != SyncResult::NONE)
			{
				runningGame.second->Clear();
				makeEmptyGameStack.push(runningGame.first);
			}
		}
		else if (status == GameStatus::PLAYING)
		{
			if (runningGame.second->PlayingSync() != SyncResult::NONE)
			{
				runningGame.second->Clear();
				makeEmptyGameStack.push(runningGame.first);
			}
		}
		else if (status == GameStatus::SOMEONE_LEANVE)
		{
			runningGame.second->LeaveSync();
			runningGame.second->Clear();
			makeEmptyGameStack.push(runningGame.first);
		}
	}
	while (makeEmptyGameStack.empty() == false)
	{
		INT32 roomId = makeEmptyGameStack.top();
		makeEmptyGameStack.pop();
		std::unique_lock<std::mutex> lock(_runningGameMutex);
		_emptyGames.push(_runningGames[roomId]);
		_runningGames.erase(roomId);
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
