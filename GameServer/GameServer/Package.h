#pragma once
#include "Packet.h"

struct Package
{
public:
	INT32 pakcetId = -1;
	INT32 sessionId = -1;
	std::vector<char> _buffer;
};

