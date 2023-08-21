#pragma once
#include "GameHandler.h"

class SpikeBeachHandler : public GameHandler
{
public:
	SpikeBeachHandler();
	~SpikeBeachHandler() {};
	bool IsRuning();
private:
	std::atomic_bool _isRunning;
	void EnterGame(Package package);
	//void LeaveGame(Package package);
	void SyncGame(Package package);
	void ControllGame(Package package);
};
