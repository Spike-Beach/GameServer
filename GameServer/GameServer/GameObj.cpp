#include "pch.h"
#include "GameObj.h"

/*
* std::get<2> : 가속도
* std::get<1> : 속도
* std::get<0> : 위치
*/
void GameObj::Sync(std::chrono::system_clock::time_point syncReqTime)
{
	Acceleration& objAcc = std::get<2>(_motionData);
	Velocity& objVel = std::get<1>(_motionData);
	Position& objPos = std::get<0>(_motionData);
	Acceleration beforeCtrlAcc(objAcc);
	Acceleration afterCtrlAcc(0);

	float beforeCtrlMaxVelMagnitude = 0;
	float afterCtrlMaxVelMagnitude = 0;

	float beforeCtrlDeltaTime = std::chrono::duration<float>(syncReqTime - _lastSyncTime).count();
	float afterCtrlDeltaTime = 0;

	std::unique_lock<std::shared_mutex> lock(_objMutex);

	while (_reservedControll.empty() == false && _reservedControll.front().first <= syncReqTime)
	{
		beforeCtrlDeltaTime = std::chrono::duration<float>(_reservedControll.front().first - _lastSyncTime).count();
		afterCtrlDeltaTime = std::chrono::duration<float>(syncReqTime - _reservedControll.front().first).count();
		
		auto dir = _reservedControll.front().second;
		_reservedControll.pop_front();
		Acceleration tempAcc(dir.first, dir.second, 0);
		afterCtrlAcc = tempAcc.GetNomalAcc() * CNTRL_ACC;
		objAcc = afterCtrlAcc;
	}

	// 컨트롤 이전에 적용된 가속도 계산(컨트롤이 없으면 싱크 전체 구간)
	std::tie(beforeCtrlAcc, beforeCtrlMaxVelMagnitude) = DetermineAccNVel(beforeCtrlAcc);
	if (beforeCtrlMaxVelMagnitude >= 0)
	{
		Velocity oldVel = objVel;
		objVel.UpdateVelocity(beforeCtrlAcc, beforeCtrlDeltaTime); // 속도 갱신
		objVel.AdjustToMaxMagnitude(beforeCtrlMaxVelMagnitude, oldVel);
		objPos.CalNewPosition(objVel, beforeCtrlDeltaTime);
	}

	// 컨트롤이 있다면
	if (afterCtrlDeltaTime > 0)
	{
		// 컨트롤 이후 적용된 가속도 계산
		std::tie(afterCtrlAcc, afterCtrlMaxVelMagnitude) = DetermineAccNVel(afterCtrlAcc);
		if (afterCtrlMaxVelMagnitude >= 0)
		{
			Velocity oldVel = objVel;
			objVel.UpdateVelocity(afterCtrlAcc, afterCtrlDeltaTime); // 속도 갱신
			objVel.AdjustToMaxMagnitude(afterCtrlMaxVelMagnitude, oldVel);
			objPos.CalNewPosition(objVel, afterCtrlDeltaTime);
		}
	}

	_lastSyncTime = syncReqTime;
	return ;
};

std::pair<Acceleration, float> GameObj::DetermineAccNVel(const Acceleration& acc)
{
	Acceleration determinedAcc(0);
	float determinedVel = -1;
	if (acc.IsZero() == true)
	{
		ThreeValues VelNomalVec = std::get<1>(_motionData).GetNomal();
		if (VelNomalVec.IsZero() == false)
		{
			determinedAcc = VelNomalVec * STOP_ACC_SCAL;
			determinedVel = 0;
		}
	}
	else
	{
		determinedAcc = acc;
		determinedVel = MAX_VEL;
	}
	return std::make_pair(determinedAcc, determinedVel);
}

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
