#pragma once
#include <ctime>
#include <iomanip>

#define g_logger Logger::Instance()

enum class LogLevel : UINT16
{
	INFO = 0,
	WARNING = 1,
	ERR = 2,
	CRITICAL = 3
};

class Logger : public Singleton<Logger>
{
public:
	void Log(LogLevel level, const std::string& callLocation, const std::string& msg);
	void PrintLogMsg(); // for task

	std::string GenNowTimeStr();
	std::string GenNowTimeStr(std::chrono::system_clock::time_point tp);
private:
	std::mutex _queueMutex;
	std::queue<std::string> waitingLogQueue;

	std::queue<std::string> writingLogQueue;

	std::string GetLogLevelStr(LogLevel level);
};

