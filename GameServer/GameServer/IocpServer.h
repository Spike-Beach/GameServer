#pragma once

#include "Server.h"
#include "IocpSession.h"
#include "SessionManager.h"

class IocpServer : public Server, public Singleton<IocpServer>
{
public:
	IocpServer(std::unique_ptr<ContentsProcess> contentsProcess);
	~IocpServer();
	void Init();
	bool Run();
private:
	bool SetListenSocket();
	void SetAcceptedSocket(SOCKET accepter, SOCKADDR_IN addrInfo);
	//void PutPackage(Package package);

	// 전역 싱글턴을 사용하기 위해 static으로 선언
	static DWORD WINAPI AcceptThreadFunc(LPVOID serverPtr);
	static DWORD WINAPI WorkerThreadFunc(LPVOID serverPtr);

	SOCKET _listenSocket;
	HANDLE _iocpHandle;
	INT32 _workerThreadCount;
	std::vector<std::thread> _workerThreads;
	std::thread _acceptThread;
	static std::atomic<bool> _isShutdown;
};

