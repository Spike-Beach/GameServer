#pragma once
#include "TaskProcesser.h"

#define g_TaskManager TaskManager::Instance()
#define TASK_PROCESSER_COUNT 5
#define USER_TASK_PROCESSER_ID 0
#define GAME_TASK_PROCESSER_ID 1
#define LOG_TASK_PROCESSER_ID 2

class TaskManager : public Singleton<TaskManager>
{
public:
	TaskManager();
	~TaskManager();
	void Init();
	void AddTask(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs, INT32 processerId);
private:
	std::unordered_map<INT32, std::unique_ptr<TaskProcesser> > _taskProcessers;
};

