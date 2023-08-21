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
	// _task == false는 컴파일러에 의해 암묵적으로 !_task로 변환
	// _task == true는 암묵적인 bool 변환이 적용되지 않는다....왜?
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