#pragma once

#include "Package.h"
#include "SessionManager.h"

class GameHandler
{
public:
	virtual ~GameHandler() {};
	void Mapping(Package package)
	{
		auto func = _contentFuncMap.find(package.pakcetId);
		if (func != _contentFuncMap.end())
		{
			func->second(package);
		}
		else
		{
			// TODO: logger, ���� ó�� �ؾ��ϴ���
			g_sessionManager.ReleaseSession(package.sessionId, true);
		}
	}
protected:
	std::map<INT32, std::function<void(Package)> > _contentFuncMap;
};

