#pragma once
#include "pch.h"
#include "Task.h"
#include "SBManager.h"
//#include "SpikeBeachHandler.h"

class GameSyncTask : public Task
{
public:
	//GameSyncTask(std::shared_ptr<SpikeBeachContent> spikeBeachContent) : _spikeBeachContent(spikeBeachContent) {};
	GameSyncTask() {};
	void Do()
	{
		//_spikeBeachContent.get()->SyncGames();
		g_SBManager.SyncGames();
	}
private:
	//std::shared_ptr<SpikeBeachContent> _spikeBeachContent;
};

