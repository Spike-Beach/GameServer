
#include "pch.h"
#include "mysql_connection.h"
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/prepared_statement.h>

#include "IocpServer.h"
#include "SpikeBeachHandler.h"
#include "TaskManager.h"
#include "SessionMonitor.h"
#include "SBUserCheckNSet.h"
#include "GameSetTask.h"
#include "GameSyncTask.h"
#include "LoggingTask.h"
#include "Logger.h"

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
		g_TaskManager.AddTask(std::move(std::make_unique<GameSetTask>()), 100, 0, GAME_TASK_PROCESSER_ID);
		//g_TaskManager.AddTask(std::move(std::make_unique<GameSyncTask>()), 100, 0, GAME_TASK_PROCESSER_ID);
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::CRITICAL, "main()", e.what());
	}
	while (1)
	{
		std::cin >> line;
		if (line == "q")
		{
			break;
		}
	}
	//std::cout << "Hello world" << std::endl;
 //   redisContext* c = redisConnect("127.0.0.1", 6379);
 //   if (c == NULL || c->err) {
 //       if (c) {
 //           printf("Error: %s\n", c->errstr);
 //           // handle error
 //       }
 //       else {
 //           printf("Can't allocate redis context\n");
 //       }
 //   }
	//return 0;
}