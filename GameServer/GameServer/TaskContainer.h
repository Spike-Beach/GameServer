#pragma once
#include "Task.h"

using MilliDuration = std::chrono::duration<int, std::milli>;

class TaskContainer
{
public:
	TaskContainer(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs);
	TaskContainer(const TaskContainer& other) = delete;
	TaskContainer(TaskContainer&& other)
		: _task(std::move(other._task)), _freqMs(other._freqMs), _expireTime(other._expireTime)
		, _nextTaskTime(other._nextTaskTime), _isRepeat(other._isRepeat)
	{}
	~TaskContainer() { _task.reset(); }
	bool IsExpired();
	void doTask();

private:
	// 포인터를 쓰는 이유 : Task를 상속받은 클래스를 사용하기 위해
	std::unique_ptr<Task> _task;
	MilliDuration _freqMs;
	bool _isRepeat;

	std::chrono::system_clock::time_point _expireTime;
	std::chrono::system_clock::time_point _nextTaskTime;
};

