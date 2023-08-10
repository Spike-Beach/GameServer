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
	void Log(LogLevel level, const std::string& callLocation, const std::string& msg)
	{
		std::string timeStr = GenTimeStr();
		std::string logLevelStr = GetLogLevelStr(level);

		std::unique_lock<std::mutex> lock(_queueMutex);
		messageQueue.push("[" + timeStr + "](" + logLevelStr + ") " + callLocation + " " + msg);
	}

	void PrintLogMsg()
	{
		std::string msg;
		std::unique_lock<std::mutex> lock(_queueMutex);
		if (!messageQueue.empty())
		{
			msg = messageQueue.front();
			messageQueue.pop();
			lock.unlock();
			std::cout << msg << std::endl;
		}
	}

private:
	std::mutex _queueMutex;
	std::condition_variable condition;
	std::queue<std::string> messageQueue;

	std::string GenTimeStr()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		struct tm localTime;
		std::ostringstream oss;
		localtime_s(&localTime, &currentTime);
		oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");

		return oss.str();
	}

	std::string GetLogLevelStr(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::WARNING:
			return "WARNING";
		case LogLevel::ERR:
			return "ERR";
		}
		return "UNKNOWN";
	}
};

