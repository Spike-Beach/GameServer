#pragma once
#include <string>
#include <memory>
#include "ContentsProcess.h"

class Package;

enum SERVER_STATUS
{
	SERVER_STOP,
	SERVER_INITIALZE,
	SERVER_READY,
};

class Server
{
public:
	Server(std::unique_ptr<ContentsProcess> contentsProcess);
	virtual ~Server();
	virtual void init();
	virtual bool run();
	SERVER_STATUS& getStatus();
	void putPackage(Package package);

protected:
	std::string _ip;
	int16_t _port;
	int16_t _workerThreadCount;
	SERVER_STATUS _status;
	std::unique_ptr<ContentsProcess> _contentsProcess;

private:
};

