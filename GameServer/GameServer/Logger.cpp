#include "pch.h"
#include "Logger.h"

void Logger::Log(LogLevel level, const std::string& callLocation, const std::string& msg)
{
	std::string timeStr = GenTimeStr();
	std::string logLevelStr = GetLogLevelStr(level);

	std::unique_lock<std::mutex> lock(_queueMutex);
	waitingLogQueue.push("[" + timeStr + "](" + logLevelStr + ") " + callLocation + " " + msg);
}

void Logger::PrintLogMsg()
{
	std::string msg;
	std::unique_lock<std::mutex> lock(_queueMutex);
	while (waitingLogQueue.empty() == false)
	{
		writingLogQueue.push(std::move(waitingLogQueue.front()));
		waitingLogQueue.pop();
	}
	lock.unlock();
	while (writingLogQueue.empty() == false)
	{
		std::cout << writingLogQueue.front() << std::endl;
		writingLogQueue.pop();
	}
}

std::string Logger::GenTimeStr()
{
	std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm localTime;
	std::ostringstream oss;
	localtime_s(&localTime, &currentTime);
	oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");

	return oss.str();
}

std::string Logger::GetLogLevelStr(LogLevel level)
{
	switch (level)
	{
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARNING:
		return "WARNING";
	case LogLevel::ERR:
		return "ERR";
	case LogLevel::CRITICAL:
		return "CRITICAL";
	}
	return "UNKNOWN";
}
