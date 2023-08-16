#pragma once
#include "pch.h"

#pragma pack(push,1)
class GameStartNtf : public Packet
{
public:
	GameStartNtf() : Packet(PacketId::GAME_START_NTF) {}
	INT64 gameStartTime;
	std::vector<char> Serialize()
	{
		packetId = PacketId::GAME_START_NTF;
		packetLength = PACKET_SIZE + sizeof(gameStartTime);
		std::vector<char> serialized = Packet::Serialize();
		serialized.reserve(packetLength);
		char* tempPtr = reinterpret_cast<char*>(&gameStartTime);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(gameStartTime));
		return serialized;
	}
	size_t Deserialize(char* buf, size_t length)
	{
		size_t offset = Packet::Deserialize(buf, length);
		gameStartTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(gameStartTime);
		return offset;
	}
};
#pragma pack(pop)