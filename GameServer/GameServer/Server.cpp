#include "pch.h"
#include "Server.h"

Server::Server(std::shared_ptr<GameHandler> gameHandler)
	//: _contentsProcess(std::move(contentsProcess))
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
	_ip = "127.0.0.1";
	_port = 8444;
	_workerThreadCount = 4;
	g_logger.Log(LogLevel::INFO, "Server::Init()", "Server Init Done");
}

//void Server::PutPackage(Package package)
//{
//	//_contentsProcess.get()->process(package);
//	return;
//}

SERVER_STATUS& Server::getStatus()
{
	return _status;
}