#include "pch.h"
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
	_sessions.reserve(MAX_SESSION_CAPACITY);
	for (size_t idx = 0; idx < CLIENT_SESSION_CAPACITY; idx++)
	{
		//IocpSession* tempSession = new IocpSession(sessionIdx++, SOCK_TYPE::IOCP);
		//_sessions.push_back(static_cast<Session*>(tempSession));
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

//INT32 SessionManager::GetSessionById(INT32 sessionId)
//{
//	if (sessionId < 0 || sessionId > _sessions.size())
//	{
//		return nullptr;
//	}
//	return _sessions[sessionId];
//}
//
//void SessionManager::ClearSession(std::shared_ptr<Session> sesstionPtr)
//{
//	std::unique_lock<std::mutex> lock(_mutex);
//	if (sessionId > 0 && sessionId < _sessions.size())
//	{
//		_sessions[sessionId]->Clear();
//		_emptyClientSessionIndexStack.push(_sessions[sessionId]);
//	}
//}

void SessionManager::SendData(INT32 idx, std::vector<char>&& serializedPacket)
{
	if (idx < 0 || idx > _sessions.size())
	{
		// log
		return;
	}
	else if (_sessions[idx]->GetStatus() != SESSION_STATUS::CONN)
	{
		return;
	}
	_sessions[idx]->SendData(std::move(serializedPacket));
}

bool SessionManager::ReleaseSession(INT32 sessionIdx, bool isForse)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (sessionIdx > 0 && sessionIdx < _sessions.size())
	{
		// TODO : forse close
		//return _sessions[sessionId]->Close(isForse);
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
	return false;
}

INT32 SessionManager::ClientSessionCap()
{
	return CLIENT_SESSION_CAPACITY;
}