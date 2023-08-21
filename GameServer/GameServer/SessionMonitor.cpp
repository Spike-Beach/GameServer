#include "pch.h"
#include "SessionManager.h"
#include "SessionMonitor.h"

void SessionMonitor::Do()
{
	auto list = g_sessionManager.GetTimeOverSessionList();
	for (auto sessionId : list)
	{
		g_sessionManager.ReleaseSession(sessionId, true);
	}
}