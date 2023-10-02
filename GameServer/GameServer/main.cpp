#include "pch.h"
#include "TaskManager.h"
#include "IocpServer.h"
#include "SpikeBeachHandler.h"
#include "SessionMonitor.h"
#include "SBUserCheckNSet.h"
#include "GameSetTask.h"
#include "GameSyncTask.h"
#include "LoggingTask.h"
#include "GameResultWriteTask.h"

int main()
{
	g_config.ReadConfig();
	IocpServer server(std::make_shared<SpikeBeachHandler>());
	server.Init();
	try
	{
		server.Run();
		g_TaskManager.AddTask(std::move(std::make_unique<SessionMonitor>()), 500, 0, USER_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<LoggingTask>()), 100, 0, LOG_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<SBUserCheckNSet>()), 100, 0, GAME_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<GameSetTask>()), 500, 0, GAME_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<GameSyncTask>()), 10, 0, GAME_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<GameResultWriteTask>()), 1'000, 0, DB_TASK_PROCESSER_ID);
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::CRITICAL, "main()", e.what());
	}
	std::string line;
	while (1)
	{
		std::cin >> line;
		if (line[0] == 'q')
		{
			break;
		}
	}
}