#include "pch.h"
#include "IocpServer.h"
#include "ServerConfigManager.h"

std::atomic<bool> IocpServer::_isShutdown = false;

IocpServer::IocpServer(std::shared_ptr<GameHandler> gameHandler)
	: Server(std::move(gameHandler))
{
	_isShutdown.store(false);
}

IocpServer::~IocpServer()
{
	_isShutdown.store(true);
	
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
	_workerThreadCount = g_config.config["ServerSettings"]["WorkerThreadCount"].asInt();
	g_logger.Log(LogLevel::INFO, "IocpServer::Init()", "IocpServer Init Done");
}

bool IocpServer::SetListenSocket()
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::SetListenSocket()", "WSASocket() failed with error code: " + std::to_string(WSAGetLastError()));
		return false;
	}

	int reUseAddr = 1;
	setsockopt(_listenSocket , SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reUseAddr), (int)sizeof(reUseAddr));

	int retval = ::bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::SetListenSocket()", "bind() failed with error code: " + std::to_string(WSAGetLastError()));
		return false;
	}

	retval = ::listen(_listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::SetListenSocket()", "listen() failed with error code: " + std::to_string(WSAGetLastError()));
		return false;
	}

	g_logger.Log(LogLevel::INFO, "IocpServer::SetListenSocket()", "SetListenSocket Done");
	return true;
}

bool IocpServer::Run()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::Run()", "WSAStartup() failed with error code: " + std::to_string(WSAGetLastError()));
		return false;
	}

	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _workerThreadCount);
	if (_iocpHandle == INVALID_HANDLE_VALUE)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::Run()", "CreateIoCompletionPort() failed with error code: " + std::to_string(WSAGetLastError()));
		return false;
	}

	if (SetListenSocket() == false) // already SetListenSocket() log fail info
	{
		return false;
	}
	_status = SERVER_STATUS::SERVER_READY;

	_acceptThread = std::thread(AcceptThreadFunc, this);
	for (int i = 0; i < _workerThreadCount; ++i)
	{
		_workerThreads.push_back(std::thread(WorkerThreadFunc, this));
	}
	
	g_logger.Log(LogLevel::INFO, "IocpServer::Run()", "IocpServer Run Done");
	return true;
}

void IocpServer::SetAcceptedSocket(SOCKET socket, SOCKADDR_IN addrInfo)
{
	IocpSession* session = SessionManager::Instance().GetEmptySession();
	if (session == nullptr)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::SetAcceptedSocket()", "Not enough session");
	}
	IocpSession* iocpSession = dynamic_cast<IocpSession*>(session);
	iocpSession->Accept(socket, addrInfo);

	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, _iocpHandle, (ULONG_PTR)iocpSession, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		g_logger.Log(LogLevel::ERR, "IocpServer::SetAcceptedSocket()", "CreateIoCompletionPort() failed with error code: " + std::to_string(WSAGetLastError()));
		g_sessionManager.ReleaseSession(iocpSession->GetId(), true);
	}

	g_logger.Log(LogLevel::INFO, "IocpServer::SetAcceptedSocket()", "cient conn. sessionId: " + std::to_string(iocpSession->GetId()));
	iocpSession->RecvTrigger();
}

DWORD WINAPI IocpServer::AcceptThreadFunc(void* serverPtr)
{
	IocpServer* server = static_cast<IocpServer*>(serverPtr);
	SOCKET acceptSocket;
	SOCKADDR_IN recvAddrInfo;

	int recvAddrInfoSize = sizeof(recvAddrInfo);
	while (_isShutdown == false)
	{
		acceptSocket = WSAAccept(server->_listenSocket, (SOCKADDR*)&recvAddrInfo, &recvAddrInfoSize, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			g_logger.Log(LogLevel::ERR, "IocpServer::AcceptThreadFunc()", "WSAAccept() failed with error code: " + std::to_string(WSAGetLastError()));
		}

		server->SetAcceptedSocket(acceptSocket, recvAddrInfo);
	}
	return 0;
}

DWORD WINAPI IocpServer::WorkerThreadFunc(void* serverPtr)
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
			g_logger.Log(LogLevel::ERR, "IocpServer::WorkerThreadFunc()", "GetQueuedCompletionStatus() failed with error code: " + std::to_string(WSAGetLastError()));
			if (iocpSession != NULL)
			{
				g_sessionManager.ReleaseSession(iocpSession->GetId(), true);
			}
			continue;
		}
		if (iocpSession == nullptr)
		{
			g_logger.Log(LogLevel::ERR, "IocpServer::WorkerThreadFunc()", "iocpSession is nullptr");
			continue;
		}
		if (transferSize == 0)
		{
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
					server->_gameHandler->Mapping(receivedPackage.value());
				}
				break;
			}
		}
	}
	return 0;
}