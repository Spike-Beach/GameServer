#include "pch.h"
#include "GameObj.h"

/*
* std::get<2> : ���ӵ�
* std::get<1> : �ӵ�
* std::get<0> : ��ġ
*/
void GameObj::Sync(std::chrono::system_clock::time_point syncReqTime)
{
	Acceleration beforeCtrlAcc(std::get<2>(_motionData));
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
		std::get<2>(_motionData) = afterCtrlAcc;
	}

	// ��Ʈ�� ������ ����� ���ӵ� ���(��Ʈ���� ������ ��ũ ��ü ����)
	std::tie(beforeCtrlAcc, beforeCtrlMaxVelMagnitude) = DetermineAccNVel(beforeCtrlAcc);
	if (beforeCtrlMaxVelMagnitude >= 0)
	{
		Velocity oldVel = std::get<1>(_motionData);
		std::get<1>(_motionData).UpdateVelocity(beforeCtrlAcc, beforeCtrlDeltaTime); // �ӵ� ����
		std::get<1>(_motionData).AdjustToMaxMagnitude(beforeCtrlMaxVelMagnitude, oldVel);
		std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), beforeCtrlDeltaTime);
	}

	// ��Ʈ���� �ִٸ�
	if (afterCtrlDeltaTime > 0)
	{
		// ��Ʈ�� ���� ����� ���ӵ� ���
		std::tie(afterCtrlAcc, afterCtrlMaxVelMagnitude) = DetermineAccNVel(afterCtrlAcc);
		if (afterCtrlMaxVelMagnitude >= 0)
		{
			Velocity oldVel = std::get<1>(_motionData);
			std::get<1>(_motionData).UpdateVelocity(afterCtrlAcc, afterCtrlDeltaTime); // �ӵ� ����
			std::get<1>(_motionData).AdjustToMaxMagnitude(afterCtrlMaxVelMagnitude, oldVel);
			std::get<0>(_motionData).CalNewPosition(std::get<1>(_motionData), afterCtrlDeltaTime);
		}
	}

	if (std::get<1>(_motionData).x != 0 || std::get<1>(_motionData).y != 0)
	{
		g_logger.Log(LogLevel::INFO, "", "Acc:" + std::to_string(std::get<2>(_motionData).x) + ", " + std::to_string(std::get<2>(_motionData).y) + ", " + std::to_string(std::get<2>(_motionData).z));
		g_logger.Log(LogLevel::INFO, "", "Vel:" + std::to_string(std::get<1>(_motionData).x) + ", " + std::to_string(std::get<1>(_motionData).y) + ", " + std::to_string(std::get<1>(_motionData).z));
		g_logger.Log(LogLevel::INFO, "", "Pos:" + std::to_string(std::get<0>(_motionData).x) + ", " + std::to_string(std::get<0>(_motionData).y) + ", " + std::to_string(std::get<0>(_motionData).z) + "\n");
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
