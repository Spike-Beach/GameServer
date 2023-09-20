#include "pch.h"
#include "GameObj.h"

/*
* std::get<2> : 가속도
* std::get<1> : 속도
* std::get<0> : 위치
*/
void GameObj::Sync(std::chrono::system_clock::time_point syncReqTime)
{
	Acceleration applyAcc(0);
	float applyMaxVelMagnitude;
	float deltaTime = std::chrono::duration<float>(syncReqTime - _lastSyncTime).count();
	std::unique_lock<std::shared_mutex> lock(_objMutex);

	while (_reservedControll.empty() == false && _reservedControll.front().first <= syncReqTime)
	{
		auto dir = _reservedControll.front().second;
		_reservedControll.pop_front();
		Acceleration tempAcc(dir.first, dir.second, 0);
		std::get<2>(_motionData) = tempAcc.GetNomalAcc() * CNTRL_ACC;
	}

	if (std::get<2>(_motionData).IsZero() == true)
	{
		ThreeValues VelNomalVec = std::get<1>(_motionData).GetNomal();
		if (VelNomalVec.IsZero() == false)
		{
			applyAcc = VelNomalVec * STOP_ACC_SCAL;
			applyMaxVelMagnitude = 0;
		}
		else
		{
			_lastSyncTime = syncReqTime;
			return ;
		}
	}
	else
	{
		applyAcc = std::get<2>(_motionData);
		applyMaxVelMagnitude = MAX_VEL;
	}

	// 속도 : 과거속도 + 현재 가속된속도. 최대속도 넘지 않게.
	// 위치 : 현재속도 * 시간.
	Velocity oldVel = std::get<1>(_motionData);
	std::get<1>(_motionData).UpdateVelocity(applyAcc, deltaTime); // 속도 갱신
	std::get<1>(_motionData).AdjustToMaxMagnitude(applyMaxVelMagnitude, oldVel);
	std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), deltaTime);

	_lastSyncTime = syncReqTime;
	return ;
};

bool GameObj::Controll(std::chrono::system_clock::time_point ctlTime, const std::pair<float, float> dir)
{
	SysTp now = std::chrono::system_clock::now();
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	if (_reservedControll.empty() == false && _reservedControll.back().second == dir)
	{
		return false;
	}
	std::pair<std::chrono::system_clock::time_point, std::pair<int, int>> ctlPair(ctlTime, dir);
	_reservedControll.push_back(ctlPair);
	return true;
}

void GameObj::clear()
{
	Reset();
	_tts = -1;
}

void GameObj::Reset()
{
	Reset(0, 0, 0);
};

void GameObj::Reset(float posX, float posY, float posZ)
{
	_motionData = { {posX, posY, posZ}, {0, 0, 0}, {0, 0, 0} };;
};

std::tuple<Position, Velocity, Acceleration> GameObj::GetMotionData()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return _motionData;
}

Position GameObj::getPosition()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return std::get<0>(_motionData);
}

Velocity GameObj::getVelocity()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return std::get<1>(_motionData);
}

Acceleration GameObj::getAcceleration()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return std::get<2>(_motionData);
}

INT64 GameObj::GetLastSyncTime()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return _lastSyncReqTime;
}

INT64 GameObj::GetTTS()
{
	std::shared_lock<std::shared_mutex> sharedLock(_objMutex);
	return _tts;
}

void GameObj::SetUserTimes(INT64 syncReqTime, INT64 tts)
{
	std::unique_lock<std::shared_mutex> lock(_objMutex);
	_lastSyncReqTime = syncReqTime;
	_tts = tts;
}
