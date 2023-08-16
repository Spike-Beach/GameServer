
#include "pch.h"
#include "IocpSession.h"

IocpSession::IocpSession(INT16 sessionId, SOCK_TYPE sessionType)
: Session(sessionId, sessionType)
{
	_sockType = SOCK_TYPE::IOCP;
	_iocpData[IO_TYPE::READ].SetIoType(IO_TYPE::READ);
	_iocpData[IO_TYPE::WRITE].SetIoType(IO_TYPE::WRITE);

}

IocpSession::~IocpSession()
{
	Close();
}

void IocpSession::Close()
{
	Session::Close();
}

void IocpSession::Clear()
{
	Session::Clear();
	_iocpData[IO_TYPE::READ].Clear();
	_iocpData[IO_TYPE::WRITE].Clear();
}

INT32 IocpSession::GetId()
{
	return _idx;
}

void IocpSession::KeepSendData(size_t transferSize)
{
	if (_iocpData[IO_TYPE::WRITE].IsNeedMoreIo(transferSize) == true)
	{
		WSABUF wsabuf = _iocpData[IO_TYPE::WRITE].GetWSABuf();
		if (wsabuf.buf == nullptr || wsabuf.len == 0)
		{
			Close();
			Clear();
			return;
		}

		DWORD sendBytes;
		DWORD flags = 0;
		WSASend(
			_socket, &wsabuf, 1, &sendBytes,
			flags, _iocpData[IO_TYPE::WRITE].GetOverlappedPtr(), nullptr
		);
	}
}

std::optional<Package> IocpSession::KeepRecvPacket(size_t transferSize)
{
	if (_iocpData[IO_TYPE::READ].IsNeedMoreIo(transferSize))
	{
		RecvTrigger();
		return std::nullopt;
	}

	Package rtPackage = _iocpData[IO_TYPE::READ].ReadPacketFromBuf();
	rtPackage.sessionId = _idx;
	return rtPackage;
}

// WSASend 완료 이전에 WSASend가 들어오는 경우 문제 발생할 듯.
// TODO: 문제 발생시 해결.
void IocpSession::SendData(std::vector<char>&& serializedPacket)
{
	_iocpData[IO_TYPE::WRITE].WriteToBuf(std::move(serializedPacket));

	DWORD sendBytes;
	DWORD flags = 0;
	WSABUF wsabuf = _iocpData[IO_TYPE::WRITE].GetWSABuf();
	if (wsabuf.buf == nullptr || wsabuf.len == 0)
	{
		Close();
		Clear();
		return;
	}

	WSASend (
		_socket, &wsabuf, 1, &sendBytes,
		flags, _iocpData[IO_TYPE::WRITE].GetOverlappedPtr(), nullptr
	);
	
	RecvTrigger();
}

void IocpSession::RecvTrigger()
{
	DWORD flags = 0;
	DWORD recvBytes;
	WSABUF wsabuf = _iocpData[IO_TYPE::READ].GetWSABuf();
	if (wsabuf.buf == nullptr || wsabuf.len == 0)
	{
		g_logger.Log(LogLevel::ERR, "IocpSession::RecvTrigger()", "wsabuf.buf == nullptr || wsabuf.len == 0");
		Close();
		Clear();
		return;
	}

	_lastHeartBeat.store(time(nullptr));
	DWORD errorCode = WSARecv(
		_socket, &wsabuf, 1, &recvBytes,
		&flags, _iocpData[IO_TYPE::READ].GetOverlappedPtr(), nullptr
	);
	INT32 errCode = WSAGetLastError();
	if (errCode != ERROR_IO_PENDING && errorCode == SOCKET_ERROR)
	{
		g_logger.Log(LogLevel::ERR, "IocpSession::RecvTrigger()", "WSARecv() failed with error: %d" + std::to_string(errCode));
		Clear();
		Close();
		return;
	}
}