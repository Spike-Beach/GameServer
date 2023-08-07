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
	//_taskProcessers.reserve(TASK_PROCESSER_COUNT);
	// 0 : USER_TASK_PROCESSER_ID
	//_taskProcessers.push_back(TaskProcesser(USER_TASK_PROCESSER_ID));
	_taskProcessers.insert(std::make_pair(USER_TASK_PROCESSER_ID, std::make_unique<TaskProcesser>(USER_TASK_PROCESSER_ID)));
	//_taskProcessers.push_back(TaskProcesser(ONLY_GAME_SET_TASK_PROCSSER_ID));
	_taskProcessers.insert(std::make_pair(GAME_TASK_PROCESSER_ID, std::make_unique<TaskProcesser>(GAME_TASK_PROCESSER_ID)));
	
	;
	for (auto iter = _taskProcessers.begin(); iter != _taskProcessers.end(); iter++)
	{
		iter->second->Run();
	}
}

void TaskManager::AddTask(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs, INT32 processerId)
{
	_taskProcessers.find(processerId);
	if (_taskProcessers.find(processerId) != _taskProcessers.end())
	{
		_taskProcessers[processerId]->AddTask(std::move(task), freqMs, expireMs);
	}
	// TODO: logger
}