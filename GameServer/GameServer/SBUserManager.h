#pragma once
#include "SpikeBeachUtils.h"

#define MAX_USER 200
#define g_SBUserManager SBUserManager::Instance()

class SBUserManager : public Singleton<SBUserManager>
{
public:
	SBUserManager()
	{
		Init();
	}
	void Init()
	{
		_userPool.resize(MAX_USER);
		for (int i = 0; i < MAX_USER; ++i)
		{
			_emptyUsers.push(&_userPool[i]);
		}
	}

	SBUser* GetEmptyUser(INT64 sessionId)
	{
		std::unique_lock<std::shared_timed_mutex> lock(_managerMutex);
		if (_emptyUsers.empty() == false)
		{
			SBUser* emptyUser = _emptyUsers.top();
			emptyUser->_sessionId = sessionId;
			_sessionId2UserMap.insert(std::make_pair(sessionId, emptyUser));
			_emptyUsers.pop();
			return emptyUser;
		}
		// log
		return nullptr;
	}

	SBUser* GetUserBySessionId(INT64 sessionId)
	{
		std::shared_lock<std::shared_timed_mutex> lock(_managerMutex);
		auto iter = _sessionId2UserMap.find(sessionId);
		if (iter != _sessionId2UserMap.end())
		{
			if (iter->second->_sessionId != sessionId)
			{
				// log
			}
			return iter->second;
		}
		return nullptr;
	}

	void ReleaseUser(SBUser* user)
	{
		INT64 sessionId = user->GetSessionId();
		user->Clear();
		user->Reset();
		if (sessionId < 0)
		{
			// log
			return;
		}
		std::unique_lock<std::shared_timed_mutex> lock(_managerMutex);
		_sessionId2UserMap.erase(sessionId);
		_emptyUsers.push(user);
	}

	void PushAuthWaitingUser(SBUser* user)
	{
		std::unique_lock<std::shared_timed_mutex> lock(_authWaitingUsersMutex);
		_authWaitingUsers.push(user);
	}

	SBUser* PopAuthWaitingUser()
	{	
		std::shared_lock<std::shared_timed_mutex> lock(_authWaitingUsersMutex);
		bool isEmpty = _authWaitingUsers.empty();
		lock.unlock();
		if (isEmpty == false)
		{
			std::unique_lock<std::shared_timed_mutex> lock(_authWaitingUsersMutex);
			SBUser* user = _authWaitingUsers.front();
			_authWaitingUsers.pop();
			return user;
		}
		return nullptr;
	}

private:
	std::vector<SBUser> _userPool;
	std::stack<SBUser*> _emptyUsers;
	//std::mutex _managerMutex;
	std::shared_timed_mutex _managerMutex;
	
	std::unordered_map<INT64, SBUser*> _sessionId2UserMap;

	std::queue<SBUser*> _authWaitingUsers;
	std::shared_timed_mutex _authWaitingUsersMutex;
};

