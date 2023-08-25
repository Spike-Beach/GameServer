#include "pch.h"
#include "GameObj.h"

SyncResult GameObj::Sync(std::chrono::system_clock::time_point syncTime)
{
	Acceleration applyAcc(0);
	float applyMaxVelMagnitude;
	float deltaTime = std::chrono::duration<float>(syncTime - _lastSyncTime).count();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	if (std::get<2>(_motionData).IsZero() == true)
	{
		applyAcc.ScalarAdd(STOP_ACC_SCAL);
		applyMaxVelMagnitude = 0;
	}
	else
	{
		applyAcc = std::get<2>(_motionData);
		applyMaxVelMagnitude = MAX_VEL;
	}

	Velocity oldVel = std::get<1>(_motionData);
	std::get<1>(_motionData).CalNewVelocity(applyAcc, deltaTime); // 속도 갱신
	float maxVelApprochElapseSec = oldVel.CalMaxVelApprochElapseSec(applyAcc, applyMaxVelMagnitude);
	if (maxVelApprochElapseSec > deltaTime) // 아직 가속중. 시간에 따라 속도 적용 및 등가속도 공식으로 위치 계산
	{
		std::get<0>(_motionData).CalNewPosition((oldVel + std::get<1>(_motionData)) / 2, deltaTime);
	}
	else if (maxVelApprochElapseSec <= EPSILON) // 이미 최대 속력에 도달함. 속도 유지 및 위치 계산
	{
		std::get<1>(_motionData).AdjustToMaxMagnitude(MAX_VEL);

		std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), deltaTime);
	}
	else // 싱크 중간에 최대속력에 도달했음. 최대 속력 적용 및 계산한 최대 속력 시간을 적용해서 위치를 계산한다.
	{
		std::get<1>(_motionData).CalNewVelocity(applyAcc, deltaTime);
		std::get<1>(_motionData).AdjustToMaxMagnitude(MAX_VEL);

		std::get<0>(_motionData).CalNewPosition((oldVel + std::get<1>(_motionData)) / 2, maxVelApprochElapseSec);
		std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), deltaTime - maxVelApprochElapseSec);
	}

	_lastSyncTime = syncTime;
	lock.unlock();
	//std::string info = std::format("A({}, {}, {}) V({}, {}, {}) P({}, {}, {})"
	//	, std::get<2>(_motionData).x, std::get<2>(_motionData).y, std::get<2>(_motionData).z
	//	, std::get<1>(_motionData).x, std::get<1>(_motionData).y, std::get<1>(_motionData).z
	//	, std::get<0>(_motionData).x, std::get<0>(_motionData).y, std::get<0>(_motionData).z
	//);
	//g_logger.Log(LogLevel::INFO, "info", info);
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

void GameObj::Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration)
{
	// TODO 시간 지연에 따라, 속도와 위치 이미 반영된 값들 적용.
	_latency = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ctlTime).count();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	std::get<2>(_motionData) = acceleration;
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

void GameObj::UpdateLatency(INT64 clientTime)
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::milliseconds nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	_latency = nowMs.count() - clientTime;
}

INT64 GameObj::GetLatency()
{
	return _latency;
}
