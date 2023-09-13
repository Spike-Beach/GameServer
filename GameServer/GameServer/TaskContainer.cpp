#include "pch.h"
#include "TaskContainer.h"

TaskContainer::TaskContainer(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs)
	: _task(std::move(task)), _freqMs(MilliDuration(freqMs))
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
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

bool TaskContainer::IsExpired()
{
	if (_isRepeat)
	{
		return false;
	}

	if (!_task || std::chrono::system_clock::now() > _expireTime)
	{
		return true;
	}
	return false;
}

void TaskContainer::doTask()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	if (_task && now > _nextTaskTime)
	{
		_task.get()->Do();
		_nextTaskTime = now + MilliDuration(_freqMs);
	}
}