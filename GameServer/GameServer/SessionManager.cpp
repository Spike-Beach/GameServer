#include "pch.h"
#include "ServerConfigManager.h"
#include "SBUserManager.h"
#include "SBManager.h"
#include "SessionManager.h"
#include "IocpSession.h"

SessionManager::SessionManager()
{
	Init();
}

SessionManager::~SessionManager()
{
	for (auto session : _sessions)
	{
		session->Close();
		session.reset();
	}
}

void SessionManager::Init()
{
	size_t sessionIdx = 0;
	_sessionNum = g_config.config["SessionSettings"]["MaxSessionNum"].asInt()
		+ g_config.config["SessionSettings"]["ExtraSessionNum"].asInt();
	_sessions.reserve(_sessionNum);
	for (size_t idx = 0; idx < _sessionNum; idx++)
	{
		auto session = std::make_shared<IocpSession>(sessionIdx++, SOCK_TYPE::IOCP);
		_sessions.push_back(session);
		_emptySessionIndexStack.push(idx);
	}
}

std::shared_ptr<Session> SessionManager::GetEmptySession()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_emptySessionIndexStack.empty())
	{
		return nullptr;
	}
	auto sessionIdx = _emptySessionIndexStack.top();
	_emptySessionIndexStack.pop();
	return _sessions[sessionIdx];
}

std::list<INT32> SessionManager::GetTimeOverSessionList()
{
	INT32 timeOverSessionId = -1;
	std::list<INT32> timeOverSessionList;
	for (auto sessionPtr : _sessions)
	{
		timeOverSessionId = sessionPtr.get()->CheckTimeOver();
		if (timeOverSessionId != -1)
		{
			timeOverSessionList.push_back(timeOverSessionId);
		}
	}
	return timeOverSessionList;
}

void SessionManager::SendData(INT32 idx, std::vector<char>&& serializedPacket)
{
	if (idx < 0 || idx > _sessions.size())
	{
		g_logger.Log(LogLevel::ERR, "SessionManager::SendData", "Invalid session index - " + std::to_string(idx));
		return;
	}
	else if (_sessions[idx]->GetStatus() != SESSION_STATUS::CONN)
	{
		return;
	}
	if (serializedPacket.size() == 0)
	{
		g_logger.Log(LogLevel::ERR, "SessionManager::SendData", "serializedPacket.size() == 0");
		return;
	}
	_sessions[idx]->SendData(std::move(serializedPacket));
}

void SessionManager::SendData(INT32 idx, std::vector<char>& serializedPacket)
{
	std::vector<char> temp(serializedPacket);
	SendData(idx, std::move(temp));
}

bool SessionManager::ReleaseSession(INT32 sessionIdx, bool isForse)
{
	SBUser* user = g_SBUserManager.GetUserBySessionId(sessionIdx);
	if (user != nullptr)
	{
		INT32 gameId = user->GetGameId();
		if (gameId > 0)
		{
			g_SBManager.UserLeaveGame(gameId, user);
			g_logger.Log(LogLevel::INFO, "SessionManager::ReleaseSession", "user " + std::to_string(user->GetId()) + " leave " + std::to_string(gameId) + "Game");
		}
	}

	std::unique_lock<std::mutex> lock(_mutex);
	if (sessionIdx < 0 && sessionIdx > _sessions.size())
	{
		g_logger.Log(LogLevel::ERR, "SessionManager::ReleaseSession", "SessionManager::ReleaseSession() : Invalid session index - " + std::to_string(sessionIdx));
		return false;
	}
	if (isForse == true)
	{
		_sessions[sessionIdx]->Close();
		_sessions[sessionIdx]->Clear();
		_emptySessionIndexStack.push(sessionIdx);
	}
	else
	{
		_sessions[sessionIdx]->WaitSelfDisconnect();
	}

	return true;
}

INT32 SessionManager::ClientSessionCap()
{
	return _sessionNum;
}