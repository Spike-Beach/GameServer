#pragma once
//#include "Package.h"
#include "SessionManager.h"

class GameHandler
{
public:
	virtual ~GameHandler() {};
	void Mapping(Package package);
protected:
	std::map<INT32, std::function<void(Package)> > _contentFuncMap;
};

