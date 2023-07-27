#include "pch.h"

#include "TaskManager.h"


TaskManager::TaskManager()
{
	Init();
}

TaskManager::~TaskManager()
{
	_taskProcessers.clear();
}

void TaskManager::Init()
{
	_taskProcessers.reserve(TASK_PROCESSER_COUNT);
	// 0 : USER_TASK_PROCESSER_ID
	_taskProcessers.push_back(TaskProcesser(USER_TASK_PROCESSER_ID));

	for (auto iter = _taskProcessers.begin(); iter != _taskProcessers.end(); iter++)
	{
		iter->Run();
	}
}

void TaskManager::AddTask(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs, INT32 processerId)
{
	if (processerId >= 0 && processerId < TASK_PROCESSER_COUNT)
	{
		_taskProcessers[processerId].AddTask(std::move(task), freqMs, expireMs);
	}
	// TODO: logger
}