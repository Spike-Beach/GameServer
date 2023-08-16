#pragma once
#include "pch.h"
#include "Task.h"
#include "SBManager.h"

class GameSyncTask : public Task
{
public:
	GameSyncTask() {};
	void Do()
	{
		g_SBManager.SyncGames();
	}
private:
};

