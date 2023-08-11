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
	
	std::unique_lock<std::mutex> lock(_managerMutex);
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

// content단에서 입장 못할시 처리.
bool SBManager::UserEnterGame(INT32 roomId, SBUser* user)
{
	std::unique_lock<std::mutex> lock(_managerMutex);
	auto iter = _runningGames.find(roomId);
	if (iter != _runningGames.end())
	{
		return iter->second->UserIn(user);
	}
	g_logger.Log(LogLevel::ERR, "SBManager::UserEnterGame", "Not found roomId : " + std::to_string(roomId));
	return false;
}

// 게임 싱크. 
//   대기중인 게임 : 게임인원 싱크 및 타임오버시 게임 초기화 및 통보
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
