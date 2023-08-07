
#include "pch.h"
#include "IocpServer.h"

std::atomic<bool> IocpServer::_isShutdown = false;

//IocpServer::IocpServer(std::unique_ptr<ContentsProcess> contentsProcess)
IocpServer::IocpServer(std::shared_ptr<GameHandler> gameHandler)
	: Server(std::move(gameHandler))
{
	//_isShutdown.store(false);
	_status = SERVER_STATUS::SERVER_INITIALZE;
}

IocpServer::~IocpServer()
{
	_isShutdown.store(true);
	_status = SERVER_STATUS::SERVER_STOP;
	_acceptThread.join();
	for (auto& workerThread : _workerThreads)
	{
		workerThread.join();
	}
	WSACleanup();
}

void IocpServer::Init()
{
	Server::Init();
	_workerThreadCount = 4;
}

bool IocpServer::SetListenSocket()
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)_port);
	inet_pton(AF_INET, _ip.c_str(), &(serverAddr.sin_addr));

	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET)
	{
		// TODO: logger
		int errorCode = WSAGetLastError();
		return false;
	}

	int reUseAddr = 1;
	setsockopt(_listenSocket , SOL_SOCKET, SO_REUSEADDR, (char*)&reUseAddr, (int)sizeof(reUseAddr));

	int retval = ::bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
	{
		// TODO: logger
		return false;
	}

	retval = ::listen(_listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		return false;
	}
	return true;
}

bool IocpServer::Run()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}
	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (_iocpHandle == INVALID_HANDLE_VALUE)
	{
		// TODO: logger
		return false;
	}
	if (SetListenSocket() == false)
	{
		// TODO: logger
		return false;
	}
	_status = SERVER_STATUS::SERVER_READY;
	_acceptThread = std::thread(AcceptThreadFunc, this);
	for (int i = 0; i < _workerThreadCount; ++i)
	{
		_workerThreads.push_back(std::thread(WorkerThreadFunc, this));
	}
	return true;
}

void IocpServer::SetAcceptedSocket(SOCKET socket, SOCKADDR_IN addrInfo)
{
	std::shared_ptr<Session> session = SessionManager::Instance().GetEmptySession();
	if (session == nullptr)
	{
		// TODO: logger
	}
	else if (session->GetSockType() != SOCK_TYPE::IOCP)
	{
		// TODO: logger
	}
	IocpSession* iocpSession = dynamic_cast<IocpSession*>(session.get());
	iocpSession->Clear();
	iocpSession->Accept(socket, addrInfo);

	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, _iocpHandle, (ULONG_PTR)iocpSession, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		// TODO: logger
		iocpSession->Close();
		iocpSession->Clear();
	}
	iocpSession->RecvTrigger();
}

DWORD WINAPI  IocpServer::AcceptThreadFunc(LPVOID serverPtr)
{
	IocpServer* server = static_cast<IocpServer*>(serverPtr);
	SOCKET acceptSocket;
	SOCKADDR_IN recvAddrInfo;
	_isShutdown.store(false);
	int recvAddrInfoSize = sizeof(recvAddrInfo);
	while (_isShutdown == false)
	{
		acceptSocket = WSAAccept(server->_listenSocket, (SOCKADDR*)&recvAddrInfo, &recvAddrInfoSize, NULL, 0);
		if (acceptSocket == INVALID_SOCKET)
		{
			int errorNum = WSAGetLastError();
			// TODO: logger
		}
		//SetAcceptedSocket(acceptSocket, recvAddrInfo); <- 에러.C++ a nonstatic member reference must be relative to a specific object
		// 1. static이 아닌 멤버함수를 사용하려면, 인스턴스가 필요.
		// 2. 지금 이 함수가 호출되는 시점은 static이므로 인스턴스 생성 이전임
		// 3. 따라서 그냥 SetAcceptedSocket()를 쓰면 인스턴스가 없어서 에러가남.
		// 4. 근데 위에서 serverPtr로 서버의 인스턴스를 받아와서 사용 가능
		// 5. 짜피, 이 함수는 인스턴스 생성 이후에만 호출되므로 에러 안남
		server->SetAcceptedSocket(acceptSocket, recvAddrInfo);
	}
	return 0;
}

DWORD WINAPI IocpServer::WorkerThreadFunc(LPVOID serverPtr)
{
	IocpServer* server = static_cast<IocpServer*>(serverPtr);
	IocpData* iocpData = nullptr;
	IocpSession* iocpSession = nullptr;
	DWORD transferSize;

	while (_isShutdown.load() == false)
	{
		bool retval = GetQueuedCompletionStatus(server->_iocpHandle, &transferSize, (PULONG_PTR)&iocpSession, (LPOVERLAPPED*)&iocpData, INFINITE);
		if (retval == false)
		{
			// TODO: logger
			continue;
		}
		if (iocpSession == nullptr)
		{
			continue;
		}
		if (transferSize == 0)
		{
			//SessionManager::Instance().CloseSession(iocpSession->GetId(), false);
			SessionManager::Instance().ReleaseSession(iocpSession->GetId(), false);
			continue;
		}

		switch (iocpData->GetIoType())
		{
			case IO_TYPE::WRITE:
			{
				iocpSession->KeepSendData(transferSize);
				break;
			}
			case IO_TYPE::READ:
			{
				std::optional<Package> receivedPackage = iocpSession->KeepRecvPacket(transferSize);
				if (receivedPackage.has_value() == true)
				{
					// package.pakcetId = PacketId::PARSE_ERROR인 경우 map에서 걸러지게.
					//server->PutPackage(receivedPackage.value());
					server->_gameHandler->Mapping(receivedPackage.value());
				}
				break;
			}

		}
	}
	return 0;
}