#include "pch.h"
#include "TaskContainer.h"

TaskContainer::TaskContainer(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs)
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

bool TaskContainer::IsExpired()
{
	if (_isRepeat)
	{
		return false;
	}
	// _task == false�� �����Ϸ��� ���� �Ϲ������� !_task�� ��ȯ
	// _task == true�� �Ϲ����� bool ��ȯ�� ������� �ʴ´�....��?
	TimePoint now = SystemClock::now();
	if (!_task || now > _expireTime)
	{
		return true;
	}
	return false;
}

void TaskContainer::doTask()
{
	TimePoint now = SystemClock::now();
	if (_task && now > _nextTaskTime)
	{
		_task.get()->Do();
		_nextTaskTime = now + MilliDuration(_freqMs);
	}
}