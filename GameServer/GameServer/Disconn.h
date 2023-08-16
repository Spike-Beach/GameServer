#pragma once
#include "pch.h"

#pragma pack(push,1)
class DicconnReq : public Packet
{
public:
	DicconnReq() : Packet(PacketId::DISCONN_REQ) {}
};

class DisconnRes : public Packet
{
public:
	DisconnRes() : Packet(PacketId::DISCONN_RES) {}
};

class DisconnNtf : public Packet
{
public:
	INT16 playerIdx;
	DisconnNtf() : Packet(PacketId::CLOSE_CONN) {}
	std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + sizeof(playerIdx);

		std::vector<char> serialized = Packet::Serialize();

		char* tempPtr = reinterpret_cast<char*>(&playerIdx);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(playerIdx));

		return serialized;
	}

	size_t Deserialize(char* buf, size_t length)
	{
		size_t offset = Packet::Deserialize(buf, length);
		playerIdx = *reinterpret_cast<INT16*>(buf + offset);
		offset += sizeof(playerIdx);
		return offset;
	}
};
#pragma pack(pop)
