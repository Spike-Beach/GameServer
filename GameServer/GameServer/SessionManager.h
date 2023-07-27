#pragma once
#include "Session.h"

#define g_sessionManager SessionManager::Instance()
#define CLIENT_SESSION_CAPACITY 2000
#define OTHER_SESSION_CAPACITY 10
#define MAX_SESSION_CAPACITY (CLIENT_SESSION_CAPACITY + OTHER_SESSION_CAPACITY)


class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager();
	~SessionManager();
	void Init();
	std::shared_ptr<Session> GetEmptySession();
	std::list<INT32> GetTimeOverSessionList();
	//void ClearSession(std::shared_ptr<Session>);
	bool ReleaseSession(INT32 Idx, bool isForse);
	INT32 ClientSessionCap();
protected:
private:
	std::vector<std::shared_ptr<Session>> _sessions;
	//std::vector<Session*> _sessions;
	std::stack<INT32> _emptySessionIndexStack;
	std::mutex _mutex;
};
