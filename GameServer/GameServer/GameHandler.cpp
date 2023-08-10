#include "pch.h"
#include "GameHandler.h"

void GameHandler::Mapping(Package package)
{
	auto func = _contentFuncMap.find(package.pakcetId);
	if (func != _contentFuncMap.end())
	{
		func->second(package);
	}
	else
	{
		g_logger.Log(LogLevel::ERR, "GameHandler::Mapping", "Not found packetId : " + std::to_string(package.pakcetId));
		g_sessionManager.ReleaseSession(package.sessionId, true);
	}
}