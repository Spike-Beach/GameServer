#include "pch.h"
#include "Session.h"

bool Session::Accept(SOCKET socket, SOCKADDR_IN addrIn)
{
	_socket = socket;
	_addrIn = addrIn;
	_lastHeartBeat.store(time(nullptr));
	_status.store(SESSION_STATUS::CONN);
	return true;
}

void Session::Close()
{
	::closesocket(_socket);
}

void Session::Clear()
{
	_socket = INVALID_SOCKET;
	ZeroMemory(&_addrIn, sizeof(_addrIn));
	_status.store(SESSION_STATUS::EMPTY);
}

void Session::WaitSelfDisconnect()
{
	if (_status.load() == SESSION_STATUS::CONN)
	{
		_status.store(SESSION_STATUS::WAIT_DISCONN);
		_selfDisconnDeadLine = time(nullptr) + TIME_OVER_SEC;
	}
}

INT32 Session::CheckTimeOver()
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

Session::Session(INT16 sessionId, SOCK_TYPE sessionType)
	: _idx(sessionId), _sockType(sessionType), _lastHeartBeat(time(nullptr)), _socket(INVALID_SOCKET), _status(SESSION_STATUS::EMPTY)
{
	ZeroMemory(&_addrIn, sizeof(_addrIn));
}

Session::Session(const Session& other)
	:_idx(other._idx), _socket(other._socket), _addrIn(other._addrIn)
	, _sockType(other._sockType)
{
	_status.store(other._status.load());
	_lastHeartBeat.store(other._lastHeartBeat.load());
	_selfDisconnDeadLine.store(other._selfDisconnDeadLine.load());
}

Session::~Session()
{
}

SOCK_TYPE Session::GetSockType()
{ 
	return _sockType;
}

SESSION_STATUS Session::GetStatus()
{ 
	return _status.load(); 
}