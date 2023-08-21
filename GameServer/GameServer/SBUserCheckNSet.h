#pragma once
#include "Task.h"

class SBUserCheckNSet : public Task
{
public:
	SBUserCheckNSet();
	void Do();

private:
	redisContext* _conn;
	std::string _ip;
	INT32 _port;
};

