#pragma once

#include <string>
#include <memory>
#include "GameHandler.h"
#include "Package.h"
//#include "Packet.h"

struct Package;
class GameHandler;

enum SERVER_STATUS
{
	SERVER_STOP,
	SERVER_INITIALZE,
	SERVER_READY,
};

class Server
{
public:
	//Server(std::unique_ptr<ContentsProcess> contentsProcess);
	Server(std::shared_ptr<GameHandler> contentsProcess);
	virtual ~Server();
	virtual void Init();
	virtual bool Run() = 0;
	SERVER_STATUS& getStatus();
	//void PutPackage(Package package);

protected:
	std::string _ip;
	int16_t _port;
	int16_t _workerThreadCount;
	SERVER_STATUS _status;
	//std::unique_ptr<ContentsProcess> _contentsProcess;
	std::shared_ptr<GameHandler> _gameHandler;

private:
};

