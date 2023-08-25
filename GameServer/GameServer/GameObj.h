#pragma once
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define MAX_ACC 2048.0f
#define MAX_VEL 400.0f
#define STOP_ACC_SCAL -2000.0f
#define EPSILON std::numeric_limits<float>::epsilon()
//#define POS(d) std::get<1>(d)
//#define VEL(d) std::get<2>(d)
//#define ACC(d) std::get<3>(d)

enum class SyncResult : UINT16
{
	NONE = 0,
	RED_SCORE = 1,
	BLUE_SCORE = 2
};

class GameObj
{
public:
	virtual SyncResult Sync(std::chrono::system_clock::time_point syncTime);

	void Reset();
	void Reset(float posX, float posY, float posZ);
	//void setPosition(const Position& position);
	//void setPosition(float x, float y, float z);
	//void setVelocity(const Velocity& velocity);
	//void setVelocity(float x, float y, float z);
	//void setAcceleration(const Acceleration& acceleration);
	//void setAcceleration(float x, float y, float z);
	void Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration);
	std::tuple<Position, Velocity, Acceleration> GetMotionData();
	Position getPosition();
	Velocity getVelocity();
	Acceleration getAcceleration();
	void UpdateLatency(INT64 clientTime);
	INT64 GetLatency();

private:
	std::chrono::system_clock::time_point _lastSyncTime;
	std::shared_mutex _objMutex;
	std::tuple<Position, Velocity, Acceleration> _motionData;
	INT64 _latency;
};
