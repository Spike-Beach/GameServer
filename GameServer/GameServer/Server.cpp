#include "Server.h"

Server::Server(std::unique_ptr<ContentsProcess> contentsProcess)
	: _contentsProcess(std::move(contentsProcess))
{
	_status = SERVER_STATUS::SERVER_INITIALZE;
	init();
}

Server::~Server()
{
	_contentsProcess.reset();
	// TODO: logger
}

void Server::init()
{
	_ip = "127.0.0.1";
	_port = 8444;
	_workerThreadCount = 4;
}

void Server::putPackage(Package package)
{
	//_contentsProcess.get()->process(package);
}

SERVER_STATUS& Server::getStatus()
{
	return _status;
}