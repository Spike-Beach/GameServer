#pragma once
#include "pch.h"

class GameLeaveReq : public Packet
{

};

class GameLeaveNtf : public Packet
{
public:
	std::string userNickname;

	std::vector<char> Serialize()
	{
		std::vector<char> serialized = Packet::Serialize();
		serialized.insert(serialized.end(), userNickname.begin(), userNickname.end());
		return serialized;
	}

	size_t Deserialize(char* buf, size_t length)
	{
		size_t offset = Packet::Deserialize(buf, length);

		if (offset == 0)
		{
			return 0;
		}
		else if (offset >= length)
		{
			return 0;
		}

		userNickname = std::string(buf + offset);
	}
};