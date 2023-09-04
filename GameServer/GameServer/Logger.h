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
	void PrintLogMsg();

private:
	std::mutex _queueMutex;
	std::condition_variable condition;
	std::queue<std::string> waitingLogQueue;
	std::queue<std::string> writingLogQueue;

	std::string GenTimeStr();
	std::string GetLogLevelStr(LogLevel level);
};

