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
	_taskProcessers.insert(std::make_pair(USER_TASK_PROCESSER_ID, std::make_unique<TaskProcesser>(USER_TASK_PROCESSER_ID)));
	_taskProcessers.insert(std::make_pair(GAME_TASK_PROCESSER_ID, std::make_unique<TaskProcesser>(GAME_TASK_PROCESSER_ID)));
	_taskProcessers.insert(std::make_pair(LOG_TASK_PROCESSER_ID, std::make_unique<TaskProcesser>(LOG_TASK_PROCESSER_ID)));
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
		g_logger.Log(LogLevel::INFO, "TaskManager::AddTask", "AddTask : " + std::to_string(processerId));
		return;
	}
	g_logger.Log(LogLevel::CRITICAL, "TaskManager::AddTask", "Not found processerId : " + std::to_string(processerId));
}