#pragma once
#include "Packet.h"
#include "Package.h"

#define TIME_OVER_SEC 10

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
	virtual ~Session();

	virtual bool Accept(SOCKET socket, SOCKADDR_IN addrIn);
	virtual void Close();
	virtual void Clear();
	SOCK_TYPE GetSockType();
	SESSION_STATUS GetStatus();

	virtual void SendData(std::vector<char> serializedPacket) = 0;
	virtual void KeepSendData(size_t transferSize) = 0;
	virtual std::optional<Package> KeepRecvPacket(size_t transferSize) = 0;
	
	void WaitSelfDisconnect();
	INT32 CheckTimeOver();

protected:
	Session(INT16 sessionId, SOCK_TYPE sessionType);
	Session(const Session& other);
	INT32 _idx;
	SOCKET _socket;
	SOCKADDR_IN _addrIn;
	SOCK_TYPE _sockType;
	std::atomic<SESSION_STATUS> _status;
	std::atomic<time_t> _lastHeartBeat;
	std::atomic<time_t> _selfDisconnDeadLine;
};

