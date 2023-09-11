#pragma once
#include "GameObj.h"
#include "Position.h"
#include "Velocity.h"

struct UserInfo
{
	INT64 id = -1;
	std::string nickName;
};
using Team = std::array<UserInfo, 2>;

struct GameResult
{
	Team winner;
	Team loser;
	std::chrono::system_clock::time_point startTime;
	std::chrono::system_clock::time_point finishTime;
};

enum class GameStatus : UINT16
{
	EMPTY = 0,
	WAITING = 1,
	PLAYING = 2,
	FINISHING = 3,
	SOMEONE_LEANVE = 4,
};

#define HIT_RADIUS_SQUAR 225 // HIT_RADIUS : 15
class HitChecker
{
public:
	void Set(SysTp hitTime, Position hitPoint, Velocity vel, float targetHeight)
	{
		_hitTime = hitTime;
		_hitPoint = hitPoint;
		_ballVel = vel;

		// 근의 공식
		_centerElapsed1 = vel.z - std::sqrtf(std::powf(vel.z, 2) - 2 * G * (targetHeight - hitPoint.z));
		_centerElapsed2 = vel.z + std::sqrtf(std::powf(vel.z, 2) - 2 * G * (targetHeight - hitPoint.z));
	}

	bool CheckInFrame(SysTp checkTime, Position userPos)
	{
		std::chrono::duration<float> duration = _hitTime - checkTime;
		float deltaTime = duration.count();
		// 근의 공식으로 나온 2가지 시간 검사
		if (CheckUserData(userPos, deltaTime, _centerElapsed1) == true)
		{
			return true;
		}
		return CheckUserData(userPos, deltaTime, _centerElapsed2);
	}

private:
	SysTp _hitTime;
	Position _hitPoint;
	Velocity _ballVel;
	float _centerElapsed1;
	float _centerElapsed2;

	bool CheckUserData(Position userPos, float userDeltaTime, float centerTime)
	{
		if (centerTime < 0)
		{
			return false;
		}

		// 칠 수 있는 원형 범위의 중심 계산
		float centerX = _hitPoint.x + _ballVel.x * centerTime;
		float centerY = _hitPoint.y + _ballVel.y * centerTime;

		// 원의 부등식 서버가 가지고 있는 유처의 위치가 칠 수 있는 위치인지 검증
		if (std::powf(userPos.x - centerX, 2) + std::powf(userPos.y - centerY, 2) < HIT_RADIUS_SQUAR)
		{
			// 현재 공의 위치가 칠 수 있는 위치로 이동했는지 검증
			float moveX = _hitPoint.x + _ballVel.x * userDeltaTime;
			float moveY = _hitPoint.y + _ballVel.y * userDeltaTime;
			return (std::powf(centerX - moveX, 2) + std::powf(centerY - moveY, 2) < HIT_RADIUS_SQUAR);
		}
		return false;
	}
};
