#include "pch.h"
#include "Ball.h"

Ball::Ball()
	:_fallenArea(CourtArea::OUT_OF_BOUNCE), _fallenElapsedTime(-1)
{
	_hitTypes.resize(10);
}

void Ball::Init(std::array<std::pair<INT64, SBUser* >, 4>* _users)
{
	Reset();
}

void Ball::Reset()
{
	std::unique_lock<std::shared_mutex> lock(_ballMutex);
	_fallenArea = CourtArea::OUT_OF_BOUNCE;
	_fallenElapsedTime = -1;
	_lastHitterIdx = -1;
}

void Ball::Clear()
{
	std::unique_lock<std::shared_mutex> lock(_ballMutex);
	Reset();
}

void Ball::AnalyzeBallTrajectory(Position pos, Velocity vel, SysTp hitTime, INT16 hitterIdx)
{
	std::unique_lock<std::shared_mutex> lock(_ballMutex);
	_hitTime = hitTime;
	_lastHitterIdx = hitterIdx;
	CalFallen(pos, vel, hitTime);
	_hitTypes[DIG_IDX].Set(hitTime, pos, vel, DIG_Z);
	_hitTypes[RECEIVE_IDX].Set(hitTime, pos, vel, RECEIVE_Z);
	_hitTypes[PASS_IDX].Set(hitTime, pos, vel, PASS_Z);
	_hitTypes[TOSS_IDX].Set(hitTime, pos, vel, TOSS_Z);
	_hitTypes[BLOCK_IDX].Set(hitTime, pos, vel, BLOCK_Z);
	_hitTypes[SPIKE_IDX].Set(hitTime, pos, vel, SPIKE_Z);
}

bool Ball::CheckHitable(SBUser* user, SysTp hitTime, INT16 hitType)
{
	std::shared_lock<std::shared_mutex> lock(_ballMutex);
	if (hitType < 0 || hitType > MAX_HIT_TYPE_IDX)
	{
		g_logger.Log(LogLevel::ERR, "Ball::CheckHitable", "Invalid hitType : " + std::to_string(hitType));
		return false;
	}
	// TODO
	float syncElapsed = std::chrono::duration<float>(_hitTime - user->GetLastSyncTime()).count();
	Position syncPos = user->getPosition();
	syncPos.CalNewPosition(user->getVelocity(), syncElapsed);
	return _hitTypes[hitType].CheckInFrame(hitTime, syncPos);
}

BallResult Ball::Sync(SysTp syncTime)
{
	std::shared_lock<std::shared_mutex> lock(_ballMutex);
	if (_lastHitterIdx < 0)
	{
		return BallResult::NONE;
	}

	std::chrono::duration<float> syncElapsed = duration_cast<std::chrono::duration<float>>(syncTime - _hitTime);
	if (syncElapsed.count() >= _fallenElapsedTime)
	{	
		if ((_lastHitterIdx < 2 && (_fallenArea == CourtArea::OUT_OF_BOUNCE || _fallenArea == CourtArea::RED_AREA)) // 레드가 실수
			|| (_lastHitterIdx >= 2 && _fallenArea == CourtArea::RED_AREA)) // 블루가 잘 쳐서 득점한 상홤
		{
			return BallResult::SCORE_BLUE;
		}
		else
		{
			return BallResult::SCORE_RED;
		}
	}
}

void Ball::CalFallen(Position pos, Velocity vel, SysTp hitTime)
{
	_fallenElapsedTime = std::sqrt((2 * pos.z) / G); // TODO 게임 상 높이 0이 바닥이 아니니 필요시 수정.
	Position fallPos(pos.x + vel.x * _fallenElapsedTime, pos.y + vel.y * _fallenElapsedTime, 0);
	_fallenArea = Court::CheckAreaInCourt(fallPos);
}