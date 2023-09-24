#pragma once
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define CNTRL_ACC 2048.0f
#define STOP_ACC_SCAL -2000.0f
#define MAX_VEL 400.0f

class GameObj
{
public:
	virtual void Sync(std::chrono::system_clock::time_point syncReqTime);
	std::pair<Acceleration, float> DetermineAccNVel(const Acceleration& acc);
	void clear();
	void Reset();
	void Reset(float posX, float posY, float posZ);

	bool Controll(std::chrono::system_clock::time_point ctlTime, const std::pair<float, float> dir);
	
	std::tuple<Position, Velocity, Acceleration> GetMotionData();
	Position getPosition();
	Velocity getVelocity();
	Acceleration getAcceleration();
	INT64 GetLastSyncTime();
	INT64 GetTTS();
	void SetUserTimes(INT64 syncReqTime, INT64 tts);

private:
	std::shared_mutex _objMutex;
	std::chrono::system_clock::time_point _lastSyncTime;
	std::chrono::system_clock::time_point _reqControllTime;
	std::deque<std::pair<SysTp, std::pair<float, float>>> _reservedControll;
	std::tuple<Position, Velocity, Acceleration> _motionData;
	INT64 _lastSyncReqTime;
	INT64 _tts;
};
