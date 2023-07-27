#pragma once

#include "Packet.h"
#include "Package.h"

#define TIME_OVER_SEC 5

enum SOCK_TYPE
{
	IOCP
};

class Session
{
public:
	Session(INT16 sessionId, SOCK_TYPE sessionType)
		: _idx(sessionId), _sockType(sessionType), _lastHeartBeat(time(nullptr)), _socket(INVALID_SOCKET), _isRuning(false)
	{
		ZeroMemory(&_addrIn, sizeof(_addrIn));
	}
	virtual ~Session() {}

	SOCK_TYPE GetSockType() { return _sockType; }
	virtual bool Accept(SOCKET socket, SOCKADDR_IN addrIn)
	{
		_socket = socket;
		_addrIn = addrIn;
		_lastHeartBeat.store(time(nullptr));
		_isRuning.store(true);
		return true;
	}
	virtual void Close()
	{
		::closesocket(_socket);
	}
	virtual void Clear()
	{
		_socket = INVALID_SOCKET;
		ZeroMemory(&_addrIn, sizeof(_addrIn));
		_isRuning.store(false);
	}

	virtual void SendPacket(Packet packet) = 0;
	virtual void KeepSendPacket(size_t transferSize) = 0;
	virtual std::optional<Package> KeepRecvPacket(size_t transferSize) = 0;
	INT32 CheckTimeOver()
	{
		
		if (_isRuning.load() == true)
		{
			time_t now = time(nullptr);
			if (now > _lastHeartBeat + TIME_OVER_SEC)
			{
				return _idx;
			}
		}
		return -1;
	}

protected:
	INT32 _idx;
	SOCKET _socket;
	SOCKADDR_IN _addrIn;
	SOCK_TYPE _sockType;
	std::atomic<bool> _isRuning;
	std::atomic<time_t> _lastHeartBeat;
};

