
#include "pch.h"
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
	std::string line;

	g_config.ReadConfig();
	IocpServer server(std::make_shared<SpikeBeachHandler>());
	server.Init();
	server.Run();
	try
	{
		//g_TaskManager.AddTask(std::move(std::make_unique<SessionMonitor>()), 10'000, 0, USER_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<LoggingTask>()), 100, 0, LOG_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<UserCheckNSet>()), 100, 0, GAME_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<GameSetTask>()), 500, 0, GAME_TASK_PROCESSER_ID);
		//g_TaskManager.AddTask(std::move(std::make_unique<GameSyncTask>()), 100, 0, GAME_TASK_PROCESSER_ID);
		g_TaskManager.AddTask(std::move(std::make_unique<GameResultWriteTask>()), 100, 0, DB_TASK_PROCESSER_ID);
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::CRITICAL, "main()", e.what());
	}
	//GameResult �׽�Ʈ��
	GameResult testResult;
	testResult.winner[0].id = 1;
	testResult.winner[1].id = 2;
	testResult.loser[0].id = 3;
	testResult.loser[1].id = 4;
	testResult.startTime = std::chrono::system_clock::now();
	testResult.finishTime = std::chrono::system_clock::now();
	g_SBManager.SetGameResult(testResult);
	while (1)
	{
		std::cin >> line;
		if (line == "q")
		{
			break;
		}
	}
}