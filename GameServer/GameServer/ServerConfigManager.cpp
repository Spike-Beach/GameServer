#include "pch.h"

bool ServerConfigManager::ReadConfig()
{
	std::ifstream configFile(CONFIG_PATH, std::ifstream::binary);

	if (configFile.is_open() == false)
	{
		g_logger.Log(LogLevel::CRITICAL, "ServerConfigManager::ReadConfig", "Config file open failed");
		return false;
	}

	Json::Reader reader;
	if (reader.parse(configFile, config) == false)
	{
		g_logger.Log(LogLevel::CRITICAL, "ServerConfigManager::ReadConfig", "Config file parsing failed");
		configFile.close();
		return false;
	}
	configFile.close();
	return false;
}

