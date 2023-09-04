#pragma once
#include "IocpSession.h"

#define g_sessionManager SessionManager::Instance()

class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager();
	~SessionManager();
	void Init();
	IocpSession* GetEmptySession();
	std::list<INT32> GetTimeOverSessionList();
	bool ReleaseSession(INT32 idx, bool isForse);
	void SendData(INT32 idx, std::vector<char> serializedPacket);

private:
	std::vector<IocpSession> _sessions;
	std::stack<INT32> _emptySessionIndexStack;
	std::mutex _mutex;
	INT32 _sessionNum;
};

