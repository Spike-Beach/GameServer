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
		auto optVelNomal = std::get<1>(_motionData).GetNomal();
		if (optVelNomal.has_value())
		{
			applyAcc = optVelNomal.value() * STOP_ACC_SCAL;
			applyMaxVelMagnitude = 0;
		}
		else
		{
			_lastSyncTime = syncTime;
			return SyncResult::NONE;
		}
	}
	else
	{
		applyAcc = std::get<2>(_motionData);
		applyMaxVelMagnitude = MAX_VEL;
	}

	Velocity oldVel = std::get<1>(_motionData);
	std::get<1>(_motionData).UpdateVelocity(applyAcc, deltaTime); // 속도 갱신
	float maxVelApprochElapseSec = oldVel.CalMaxVelApprochElapseSec(applyAcc, applyMaxVelMagnitude);
	if (maxVelApprochElapseSec > deltaTime) // 아직 가속중. 시간에 따라 속도 적용 및 등가속도 공식으로 위치 계산
	{
		std::get<0>(_motionData).CalNewPosition((oldVel + std::get<1>(_motionData)) / 2, deltaTime);
	}
	else if (maxVelApprochElapseSec <= EPSILON) // 이미 최대 속력에 도달함. 속도 유지 및 위치 계산
	{
		std::get<1>(_motionData).AdjustToMaxMagnitude(applyMaxVelMagnitude, oldVel);
		std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), deltaTime);
	}
	else // 싱크 중간에 최대속력에 도달했음. 최대 속력 적용 및 계산한 최대 속력 시간을 적용해서 위치를 계산한다.
	{
		std::get<1>(_motionData).UpdateVelocity(applyAcc, deltaTime);
		std::get<1>(_motionData).AdjustToMaxMagnitude(applyMaxVelMagnitude, oldVel);

		std::get<0>(_motionData).CalNewPosition((oldVel + std::get<1>(_motionData)) / 2, maxVelApprochElapseSec);
		std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), deltaTime - maxVelApprochElapseSec);
	}

	_lastSyncTime = syncTime;
	lock.unlock();
	return SyncResult::NONE;
};

void GameObj::clear()
{
	Reset();
	_latency = -1;
}

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
//} 공 : 전달받는 값 - 시간, 위치, 속도 / 가지고 있을 값 : 각 모션이 가능한 시간 계산.
// 가지고 있는 값을 바탕으로 공을 칠 수 있는 지 판단. + 서버가 스파이크, ~~할 수 있는 시간대 인지 파악

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
	INT64 nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	_latency = nowMs - clientTime;
}

void GameObj::SetLastSyncTime(std::chrono::system_clock::time_point syncTime)
{
	_lastSyncTime = syncTime;
}

INT64 GameObj::GetLatency()
{
	return _latency;
}
