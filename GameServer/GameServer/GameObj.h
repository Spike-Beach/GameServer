#pragma once
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

enum class SyncResult : UINT16
{
	NONE = 0,
	RED_SCORE = 1,
	BLUE_SCORE = 2
};

class GameObj
{
public:
	virtual SyncResult Sync();

	void Reset();
	void Reset(float posX, float posY, float posZ);
	//void setPosition(const Position& position);
	//void setPosition(float x, float y, float z);
	//void setVelocity(const Velocity& velocity);
	//void setVelocity(float x, float y, float z);
	//void setAcceleration(const Acceleration& acceleration);
	//void setAcceleration(float x, float y, float z);
	void Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration, const Acceleration& stopAccel);
	std::tuple<Position, Velocity, Acceleration> GetMotionData();
	Position getPosition();
	Velocity getVelocity();
	Acceleration getAcceleration();

private:
	std::shared_mutex _objMutex;
	std::tuple<Position, Velocity, Acceleration> _motionData;
	Acceleration _stopAccel;
};
