#include "pch.h"
#include "GameObj.h"

void GameObj::Sync(std::chrono::system_clock::time_point syncTime)
{
	Acceleration applyAcc(0);
	float applyMaxVelMagnitude;
	float deltaTime = std::chrono::duration<float>(syncTime - _lastSyncTime).count();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	if (_reservedControll.has_value() && _reservedControll.value().first > syncTime)
	{
		std::get<2>(_motionData) = _reservedControll.value().second;
		_reservedControll = std::nullopt;
	}

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
			return ;
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
	return ;
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
	_motionData = { {posX, posY, posZ}, {0, 0, 0}, {0, 0, 0} };;
};

void GameObj::Controll(std::chrono::system_clock::time_point ctlTime, const Acceleration& acceleration)
{
	SysTp now = std::chrono::system_clock::now();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	if (now >= ctlTime)
	{
		std::get<2>(_motionData) = acceleration;
	}
	else
	{
		std::pair<std::chrono::system_clock::time_point, Acceleration> ctlPair(ctlTime, acceleration);
		_reservedControll = ctlPair;
	}
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

SysTp GameObj::GetLastSyncTime()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return _lastSyncTime;
}

void GameObj::UpdateLatency(INT64 clientTime)
{
	INT64 nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	g_logger.Log(LogLevel::INFO, "GameObj::UpdateLatency", "now:" + std::to_string(nowMs) + "/ clie:" + std::to_string(clientTime));
	_latency = nowMs - clientTime;
}

void GameObj::SetLastSyncTime(std::chrono::system_clock::time_point syncTime)
{
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	_lastSyncTime = syncTime;
}

INT64 GameObj::GetLatency()
{
	std::shared_lock<std::shared_mutex> lock(_objMutex);
	return _latency;
}
