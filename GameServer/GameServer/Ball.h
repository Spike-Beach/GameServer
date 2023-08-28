#pragma once
#include "SBUser.h"

class Ball
{
public:
	void Init(std::array<std::pair<INT64, SBUser* >, 4>* _users);
	void Reset();
	void Clear();
	SyncResult Sync(std::chrono::system_clock::time_point syncTime);
	bool Hit(std::chrono::system_clock::time_point hitTime);
};

