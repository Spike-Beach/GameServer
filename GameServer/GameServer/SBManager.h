#pragma once
#include "SpikeBeachGame.h"

#define g_SBManager SBManager::Instance()

class SBManager : public Singleton<SBManager>, public std::enable_shared_from_this<SBManager>
{
public:
	SBManager();
	~SBManager();
	bool Init();

	bool SetGame(std::string gameInfoStr);
	bool UserEnterGame(INT32 roomId, SBUser* user);
	void SyncGames();
	bool IsRuning();
private:
	std::atomic_bool _isRunning;
	std::vector<SpikeBeachGame> _gamePool;
	
	std::stack<SpikeBeachGame*> _emptyGames;
	// room id 2 game
	std::unordered_map<INT32, SpikeBeachGame*> _runningGames;
	std::mutex _managerMutex;
	INT32 _poolSize;
};

