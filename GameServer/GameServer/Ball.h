#pragma once
#include "SBUser.h"
#include "Court.h"
#include "SBUtils.h"

#define DIG_IDX 0
#define RECEIVE_IDX 1
#define PASS_IDX 2
#define TOSS_IDX 3
#define BLOCK_IDX 4
#define SPIKE_IDX 5

#define MAX_HIT_TYPE_IDX SPIKE_IDX

#define DIG_Z 30
#define RECEIVE_Z 45
#define PASS_Z 45
#define TOSS_Z 88
#define BLOCK_Z 173
#define SPIKE_Z 176

enum class BallResult : INT16
{
	NONE, BOUNCE, SCORE_RED, SCORE_BLUE
};

class Ball
{
public:
	Ball();
	void Init(std::array<std::pair<INT64, SBUser* >, 4>* _users);
	void Reset();
	void Clear();
	// 위치, 속도, 시간
	void AnalyzeBallTrajectory(Position pos, Velocity vel, SysTp hitTime, INT16 hitterIdx);
	bool CheckHitable(SBUser* user, SysTp hitTime, INT16 hitType);
	BallResult Sync(SysTp syncTime);
private:
	void CalFallen(Position pos, Velocity vel, SysTp hitTime);
	std::vector<HitChecker> _hitTypes;
	INT16 _lastHitterIdx;
	SysTp _hitTime;
	float _fallenElapsedTime;
	CourtArea _fallenArea;
	std::shared_mutex _ballMutex;
	// 서버로 부터 
	//  team정보? 같은것도 가지고 있어야 한다. 마지막에 친 팀이 어떤 팀인지.
	// 슬라이딩(아래), 언더 세트, 손을 아애 밑으로 받기, 중간에서 받기, 위에서 토스, 스파이크
	// -> 공격중에 할수있는게, 디그(슬라이딩), 패스, 토스(오버핸드), 스파이크
	// 블록은 네트에 팅겨서 나오는 것 과 동일하게 판정하면 된다
};

