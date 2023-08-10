#pragma once
#include "TaskContainer.h"

class TaskProcesser
{
public:
	TaskProcesser(INT32 ProcesserId);
	~TaskProcesser();
	void AddTask(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs);
	void Shutdown();
	void Run();
private:
	void TaskThread();
	INT32 _idx;
	std::atomic_bool _isShutdown;
	std::list<TaskContainer> _taskList;
	std::thread _taskThread;
	std::queue<TaskContainer> _taskWaitQueue;
	std::mutex _taskWaitQueueMutex;
};

