#include "pch.h"
#include "Server.h"
#include "ServerConfigManager.h"

Server::Server(std::shared_ptr<GameHandler> gameHandler)
	: _gameHandler(std::move(gameHandler))
{
	_status = SERVER_STATUS::SERVER_INITIALZE;
}

Server::~Server()
{
	_status = SERVER_STATUS::SERVER_STOP;
	_gameHandler.reset();
}

void Server::Init()
{
	_port = g_config.config["ServerSettings"]["Port"].asInt();
	g_logger.Log(LogLevel::INFO, "Server::Init()", "Server Init Done");
}

SERVER_STATUS& Server::getStatus()
{
	return _status;
}