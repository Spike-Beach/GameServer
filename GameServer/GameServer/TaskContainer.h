#pragma once
#include <chrono>
#include "Task.h"

using MilliDuration = std::chrono::duration<int, std::milli>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
using SystemClock = std::chrono::system_clock;

class TaskContainer
{
public:
	TaskContainer(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs)
		: _task(std::move(task)), _freqMs(MilliDuration(freqMs))
	{
		TimePoint now = SystemClock::now();
		if (expireMs == 0)
		{
			_isRepeat = true;
		}
		else
		{
			_isRepeat = false;
			_expireTime = now + MilliDuration(expireMs);
		}
		_nextTaskTime = now + MilliDuration(_freqMs);
	}
	TaskContainer(const TaskContainer& other) = delete;
	TaskContainer(TaskContainer&& other)
		: _task(std::move(other._task)), _freqMs(other._freqMs), _expireTime(other._expireTime)
		, _nextTaskTime(other._nextTaskTime), _isRepeat(other._isRepeat)
	{}
	//TaskContainer(const TaskContainer &other)
	//	: _task(std::move(other._task))
	//{
	//	_freqMs = other._freqMs;
	//	_expireTime = other._expireTime;
	//	_nextTaskTime = other._nextTaskTime;
	//	_isRepeat = other._isRepeat;
	//	//std::move(other._task);
	//}

	~TaskContainer() { _task.reset(); }

	bool IsExpired()
	{
		if (_isRepeat)
		{
			return false;
		}
		// _task == false는 컴파일러에 의해 암묵적으로 !_task로 변환
		// _task == true는 암묵적인 bool 변환이 적용되지 않는다....왜?
		TimePoint now = SystemClock::now();
		if (!_task || now > _expireTime)
		{
			return true;
		}
		return false;
	}

	void doTask()
	{
		TimePoint now = SystemClock::now();
		if (_task && now > _nextTaskTime)
		{
			_task.get()->Do();
			_nextTaskTime = now + MilliDuration(_freqMs);
		}
	}

private:
	// 포인터를 쓰는 이유 : Task를 상속받은 클래스를 사용하기 위해
	std::unique_ptr<Task> _task;
	MilliDuration _freqMs;
	bool _isRepeat;
	TimePoint _expireTime;
	TimePoint _nextTaskTime;
};

