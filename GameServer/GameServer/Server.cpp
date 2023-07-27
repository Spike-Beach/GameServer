#include "pch.h"
#include "Server.h"

Server::Server(std::unique_ptr<ContentsProcess> contentsProcess)
	: _contentsProcess(std::move(contentsProcess))
{
	_status = SERVER_STATUS::SERVER_INITIALZE;
}

Server::~Server()
{
	_contentsProcess.reset();
	// TODO: logger
}

void Server::Init()
{
	_ip = "127.0.0.1";
	_port = 8444;
	_workerThreadCount = 4;
}

void Server::PutPackage(Package package)
{
	//_contentsProcess.get()->process(package);
	return;
}

SERVER_STATUS& Server::getStatus()
{
	return _status;
}