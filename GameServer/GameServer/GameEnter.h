#pragma once
#include "pch.h"
#include "Packet.h"

#pragma pack(push,1)

class GameEnterReq : public Packet
{
public:
	GameEnterReq() : Packet(PacketId::GAME_ENTER_REQ) {}
	std::string userAssignedId;
	std::string token;
	std::string clientVersion;
	INT32 gameId;

	size_t Deserialize(char* buf, size_t length)
	{
		size_t offset = Packet::Deserialize(buf, length);
		
		userAssignedId = std::string(buf + offset);
		offset += userAssignedId.size() + 1; // +1 to skip the '\0' delimiter

		// Parse token
		token = std::string(buf + offset);
		offset += token.size() + 1;

		// Parse clientVersion
		clientVersion = std::string(buf + offset);
		offset += clientVersion.size() + 1;

		// Parse gameId
		gameId = *reinterpret_cast<INT32*>(buf + offset);
		offset += sizeof(gameId);

		return offset;
	}
	
	std::vector<char> Serialize()
	{
		packetId = PacketId::GAME_ENTER_REQ;
		packetLength = sizeof(packetId) + sizeof(packetLength) +
			userAssignedId.size() + token.size() + clientVersion.size() + 3
			+ sizeof(gameId);
		std::vector<char> serialized = Packet::Serialize();
		serialized.reserve(packetLength);

		serialized.insert(serialized.end(), userAssignedId.begin(), userAssignedId.end());
		serialized.push_back('\0');
		
		serialized.insert(serialized.end(), token.begin(), token.end());
		serialized.push_back('\0');
		
		serialized.insert(serialized.end(), clientVersion.begin(), clientVersion.end());
		serialized.push_back('\0');

		char* tempPtr = reinterpret_cast<char*>(&gameId);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(gameId));
		
		return serialized;
	}
};

class GameEnterRes : Packet
{
public:
	GameEnterRes() : Packet(PacketId::GAME_ENTER_RES) {}
	ErrorCode errorCode;
	std::vector<char> Serialize()
	{
		packetLength = sizeof(packetId) + sizeof(packetLength) + sizeof(errorCode);
		
		std::vector<char> serialized = Packet::Serialize();
		
		char* tempPtr = reinterpret_cast<char*>(&errorCode);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(errorCode));
		
		return serialized;
	}
};

#pragma pack(pop)