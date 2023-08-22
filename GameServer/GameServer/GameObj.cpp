#include "pch.h"
#include "GameObj.h"

SyncResult GameObj::Sync()
{
	// TODO. use Stop accel
	return SyncResult::NONE;
};

void GameObj::Reset()
{
	Reset(0, 0, 0);
};

void GameObj::Reset(float posX, float posY, float posZ)
{
	_motionData = { { posX, posY, posZ }, { 0, 0, 0 }, { 0, 0, 0 } };
};

//void GameObj::setPosition(const Position& position)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	std::get<0>(_motionData) = position;
//}
//
//void GameObj::setPosition(float x, float y, float z)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	//std::get<0>(_motionData) = { x, y, z };
//	std::get<0>(_motionData).x = x;
//	std::get<0>(_motionData).y = y;
//	std::get<0>(_motionData).z = z;
//}
//
//void GameObj::setVelocity(const Velocity& velocity)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	std::get<1>(_motionData) = velocity;
//}
//
//void GameObj::setVelocity(float x, float y, float z)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	//std::get<1>(_motionData) = { x, y, z };
//	std::get<1>(_motionData).x = x;
//	std::get<1>(_motionData).y = y;
//	std::get<1>(_motionData).z = z;
//}
//
//void GameObj::setAcceleration(const Acceleration& acceleration)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	std::get<2>(_motionData) = acceleration;
//}
//
//void GameObj::setAcceleration(float x, float y, float z)
//{
//	std::unique_lock<std::shared_mutex> lock(_objMutex);
//	//std::get<2>(_motionData) = { x, y, z }; 
//	std::get<2>(_motionData).x = x;
//	std::get<2>(_motionData).y = y;
//	std::get<2>(_motionData).z = z;
//}

void GameObj::Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration, const Acceleration& stopAccel)
{
	// TODO 시간 지연에 따라, 속도와 위치 이미 반영된 값들 적용.
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	std::get<2>(_motionData) = acceleration;
	_stopAccel = stopAccel;
}

std::tuple<Position, Velocity, Acceleration> GameObj::GetMotionData()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return _motionData;
}

Position GameObj::getPosition()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return std::get<0>(_motionData);
}

Velocity GameObj::getVelocity()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return std::get<1>(_motionData);
}

Acceleration GameObj::getAcceleration()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return std::get<2>(_motionData);
}
