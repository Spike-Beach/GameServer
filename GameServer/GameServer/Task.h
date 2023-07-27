#pragma once

class Task
{
public:
	virtual void Do() = 0;
	virtual void Wakeup() {};
	virtual void Suspend() {};
	virtual void Stop() {};
	virtual void Start() {};
};