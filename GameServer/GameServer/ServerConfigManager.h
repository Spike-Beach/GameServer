#pragma once

#define CONFIG_PATH "./config/serverConfig.json"
#define g_config ServerConfigManager::Instance()

class ServerConfigManager : public Singleton<ServerConfigManager>
{
public:
	Json::Value config;
	bool ReadConfig();
};

