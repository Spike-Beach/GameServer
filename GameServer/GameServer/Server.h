#pragma once
#include <string>
#include <memory>
#include "GameHandler.h"

enum SERVER_STATUS
{
	SERVER_STOP,
	SERVER_INITIALZE,
	SERVER_READY,
};

class Server
{
public:
	Server(std::shared_ptr<GameHandler> contentsProcess);
	virtual ~Server();
	virtual void Init();
	virtual bool Run() = 0;
	SERVER_STATUS& getStatus();

protected:
	INT32 _port;
	SERVER_STATUS _status;
	std::shared_ptr<GameHandler> _gameHandler;
};

