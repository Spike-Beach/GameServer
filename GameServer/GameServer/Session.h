#pragma once

#include "Packet.h"
#include "Package.h"

#define TIME_OVER_SEC 5

enum SESSION_STATUS : INT16
{
	EMPTY,
	CONN,
	WAIT_DISCONN,
};

enum SOCK_TYPE : INT16
{
	IOCP
};

class Session
{
public:
	Session(INT16 sessionId, SOCK_TYPE sessionType)
		: _idx(sessionId), _sockType(sessionType), _lastHeartBeat(time(nullptr)), _socket(INVALID_SOCKET), _status(SESSION_STATUS::EMPTY)
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
		_status.store(SESSION_STATUS::CONN);
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
		_status.store(SESSION_STATUS::EMPTY);
	}

	virtual void SendData(std::vector<char>&& serializedPacket) = 0;
	virtual void KeepSendData(size_t transferSize) = 0;
	virtual std::optional<Package> KeepRecvPacket(size_t transferSize) = 0;
	
	void WaitSelfDisconnect()
	{
		if (_status.load() == SESSION_STATUS::CONN)
		{
			_status.store(SESSION_STATUS::WAIT_DISCONN);
			_selfDisconnDeadLine = time(nullptr) + TIME_OVER_SEC;
		}
	}

	INT32 CheckTimeOver()
	{
		if (_status.load() == SESSION_STATUS::CONN)
		{
			time_t now = time(nullptr);
			if (now > _lastHeartBeat + TIME_OVER_SEC)
			{
				return _idx;
			}
		}
		else if (_status.load() == SESSION_STATUS::WAIT_DISCONN)
		{
			time_t now = time(nullptr);
			if (now > _selfDisconnDeadLine)
			{
				return _idx;
			}
		}
		return -1;
	}
	SESSION_STATUS GetStatus() { return _status.load(); }

protected:
	INT32 _idx;
	SOCKET _socket;
	SOCKADDR_IN _addrIn;
	SOCK_TYPE _sockType;
	std::atomic<SESSION_STATUS> _status;
	std::atomic<time_t> _lastHeartBeat;
	std::atomic<time_t> _selfDisconnDeadLine;
};

