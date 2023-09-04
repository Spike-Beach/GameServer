#pragma once
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define MAX_ACC 2048.0f
#define MAX_VEL 400.0f
#define STOP_ACC_SCAL -2000.0f

class GameObj
{
public:
	virtual void Sync(std::chrono::system_clock::time_point syncTime);
	void clear();
	void Reset();
	void Reset(float posX, float posY, float posZ);

	void Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration);
	
	std::tuple<Position, Velocity, Acceleration> GetMotionData();
	Position getPosition();
	Velocity getVelocity();
	Acceleration getAcceleration();
	SysTp GetLastSyncTime();

	void UpdateLatency(INT64 clientTime);
	void SetLastSyncTime(std::chrono::system_clock::time_point syncTime);
	INT64 GetLatency();

private:
	std::chrono::system_clock::time_point _lastSyncTime;
	std::chrono::system_clock::time_point _reqControllTime;
	std::optional<std::pair<SysTp, Acceleration>> _reservedControll;
	std::tuple<Position, Velocity, Acceleration> _motionData;
	std::shared_mutex _objMutex;
	INT64 _latency;
};
