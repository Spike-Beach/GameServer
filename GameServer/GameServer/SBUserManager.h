#pragma once
#include "SBUtils.h"

//#define MAX_USER 200
#define g_SBUserManager SBUserManager::Instance()

class SBUserManager : public Singleton<SBUserManager>
{
public:
	SBUserManager();
	void Init();
	SBUser* GetEmptyUser(INT64 sessionId);
	SBUser* GetUserBySessionId(INT64 sessionId);
	void ReleaseUser(SBUser* user);
	void PushAuthWaitingUser(SBUser* user);
	SBUser* PopAuthWaitingUser();

private:
	INT32 _userNum;
	std::vector<SBUser> _userPool;
	std::stack<SBUser*> _emptyUsers;
	std::shared_timed_mutex _runningGameMutex;
	std::unordered_map<INT64, SBUser*> _sessionId2UserMap;
	std::queue<SBUser*> _authWaitingUsers;
	std::shared_timed_mutex _authWaitingUsersMutex;
};

