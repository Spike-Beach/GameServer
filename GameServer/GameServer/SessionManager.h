#pragma once
#include "Session.h"

#define g_sessionManager SessionManager::Instance()

class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager();
	~SessionManager();
	void Init();
	std::shared_ptr<Session> GetEmptySession();
	std::list<INT32> GetTimeOverSessionList();
	void SendData(INT32 idx, std::vector<char>&& serializedPacket);
	void SendData(INT32 idx, std::vector<char>& serializedPacket);
	bool ReleaseSession(INT32 idx, bool isForse);
	INT32 ClientSessionCap();
private:
	std::vector<std::shared_ptr<Session> > _sessions;
	std::stack<INT32> _emptySessionIndexStack;
	std::mutex _mutex;
	INT32 _sessionNum;
};

