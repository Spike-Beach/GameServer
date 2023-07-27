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
		// _task == false�� �����Ϸ��� ���� �Ϲ������� !_task�� ��ȯ
		// _task == true�� �Ϲ����� bool ��ȯ�� ������� �ʴ´�....��?
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
	// �����͸� ���� ���� : Task�� ��ӹ��� Ŭ������ ����ϱ� ����
	std::unique_ptr<Task> _task;
	MilliDuration _freqMs;
	bool _isRepeat;
	TimePoint _expireTime;
	TimePoint _nextTaskTime;
};

