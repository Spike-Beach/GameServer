#pragma once
#include "pch.h"

class GameLeaveReq : public Packet {};

class GameLeaveRes : public Packet
{
public:
	ErrorCode errorCode;

	std::vector<char> Serialize()
	{
		std::vector<char> serialized = Packet::Serialize();
		char* tempPtr = reinterpret_cast<char*>(&errorCode);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(errorCode));
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
		errorCode = *reinterpret_cast<ErrorCode*>(buf + offset);
		offset += sizeof(ErrorCode);
		return offset;
	}
};

class GameLeaveNtf : public Packet
{
public:
	INT16 userIdx;

	std::vector<char> Serialize()
	{
		std::vector<char> serialized = Packet::Serialize();
		char* tempPtr = reinterpret_cast<char*>(&userIdx);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(INT16));
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

		userIdx = *reinterpret_cast<INT16*>(buf + offset);
		offset += sizeof(INT16);
		return offset;
	}
};