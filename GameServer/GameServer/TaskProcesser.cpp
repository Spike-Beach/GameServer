#include "pch.h"
#include "TaskProcesser.h"

TaskProcesser::TaskProcesser(INT32 ProcesserId)
	: _idx(ProcesserId), _isShutdown(false) 
{}

TaskProcesser::~TaskProcesser()
{
	if (_taskThread.joinable())
	{
		_taskThread.join();
	}
}

void TaskProcesser::AddTask(std::unique_ptr<Task> task, INT32 freqMs, INT32 expireMs)
{
	std::lock_guard<std::mutex> lock(_taskWaitQueueMutex);
	TaskContainer taskContainer(std::move(task), freqMs, expireMs);
	_taskWaitQueue.push(std::move(taskContainer));
}

void TaskProcesser::Shutdown()
{
	_isShutdown.store(true);
}

void TaskProcesser::Run()
{
	_taskThread = std::thread(&TaskProcesser::TaskThread, this);
}

void TaskProcesser::TaskThread()
{
	while (!_isShutdown)
	{
		{
			std::lock_guard<std::mutex> lock(_taskWaitQueueMutex);
			while (!_taskWaitQueue.empty())
			{
				_taskList.push_back(std::move(_taskWaitQueue.front()));
				_taskWaitQueue.pop();
			}
		}
		for (auto iter = _taskList.begin(); iter != _taskList.end();)
		{
			if (iter->IsExpired() == true)
			{
				iter = _taskList.erase(iter);
			}
			else
			{
				iter->doTask();
				iter++;
			}
		}
		::SwitchToThread(); 
	}
}