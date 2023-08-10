#pragma once
#include "GameHandler.h"
#include "SpikeBeachGame.h"
#include "TaskManager.h"
#include "SBUtils.h"

class SpikeBeachHandler : public GameHandler
{
public:
	SpikeBeachHandler();
	~SpikeBeachHandler() {};
	bool IsRuning();
private:
	std::atomic_bool _isRunning;
	void EnterGame(Package package);
};
