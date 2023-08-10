#pragma once
#include "Task.h"
#include "Logger.h"

class LoggingTask : public Task
{
public:
	void Do()
	{
		g_logger.PrintLogMsg();
	}
};
