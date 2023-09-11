#include "pch.h"
#include "SBUserManager.h"
#include "SBUser.h"
#include "ServerConfigManager.h"

SBUserManager::SBUserManager()
{
	Init();
}

void SBUserManager::Init()
{
	_userNum = g_config.config["GameSettings"]["GameUserCount"].asInt();
	_userPool.reserve(_userNum);
	for (int i = 0; i < _userNum; ++i)
	{
		_userPool.push_back(SBUser());
		_emptyUsers.push(&_userPool[i]);
	}
}

SBUser* SBUserManager::GetEmptyUser(INT64 sessionId)
{
	std::unique_lock<std::shared_timed_mutex> lock(_runningGameMutex);
	if (_emptyUsers.empty() == false)
	{
		SBUser* emptyUser = _emptyUsers.top();
		emptyUser->_sessionId = sessionId;
		_sessionId2UserMap.insert(std::make_pair(sessionId, emptyUser));
		_emptyUsers.pop();
		return emptyUser;
	}
	g_logger.Log(LogLevel::ERR, "SBUserManager::GetEmptyUser", "Not enough emptyUser");
	return nullptr;
}

SBUser* SBUserManager::GetUserBySessionId(INT64 sessionId)
{
	std::shared_lock<std::shared_timed_mutex> lock(_runningGameMutex);
	auto iter = _sessionId2UserMap.find(sessionId);
	if (iter != _sessionId2UserMap.end())
	{
		if (iter->second->_sessionId != sessionId)
		{
			g_logger.Log(LogLevel::CRITICAL, "SBUserManager::GetUserBySessionId", "sessionId is not matched. our :"
				+ std::to_string(iter->second->_sessionId) + ", req :" + std::to_string(sessionId));
		}
		return iter->second;
	}
	return nullptr;
}

void SBUserManager::ReleaseUser(SBUser* user)
{
	INT64 sessionId = user->GetSessionId();
	user->Clear();
	user->Reset();
	if (sessionId < 0)
	{
		g_logger.Log(LogLevel::ERR, "SBUserManager::ReleaseUser", "sessionId is invalid. sessionId : " + std::to_string(sessionId));
		return;
	}
	std::unique_lock<std::shared_timed_mutex> lock(_runningGameMutex);
	_sessionId2UserMap.erase(sessionId);
	_emptyUsers.push(user);
}

void SBUserManager::PushAuthWaitingUser(SBUser* user)
{
	std::unique_lock<std::shared_timed_mutex> lock(_authWaitingUsersMutex);
	_authWaitingUsers.push(user);
}

SBUser* SBUserManager::PopAuthWaitingUser()
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
