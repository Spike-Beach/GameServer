#include "pch.h"
#include "Packet.h"

INT32 Packet::ParsePacketLength(char* base, size_t bufSize)
{
	if (bufSize < sizeof(packetLength))
	{
		return 0;
	}
	INT32 packetLength = *reinterpret_cast<INT32*>(base);
	return packetLength;
}

PacketId Packet::GetPacketId(char* base, size_t bufSize)
{
	if (bufSize < sizeof(packetLength) + sizeof(packetId))
	{
		return PacketId::ERROR_OCCUR;
	}
	INT32 packetId = *reinterpret_cast<INT32*>(base + sizeof(packetLength));
	return (PacketId)packetId;
}

std::vector<char> Packet::Serialize()
{
	std::vector<char> serialized;

	//INT32 _packetLength;
	//INT32 _packetId;

	char* tempPtr = reinterpret_cast<char*>(&packetLength);
	serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(packetLength));

	tempPtr = reinterpret_cast<char*>(&packetId);
	serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(packetId));

	return serialized;
}


size_t Packet::Deserialize(char* buf, size_t length)
{
	size_t totalSize = sizeof(packetLength) + sizeof(packetId);
	if (length < totalSize)
	{
		packetId = PacketId::ERROR_OCCUR;
		return 0;
	}

	packetLength = *reinterpret_cast<INT32*>(buf);
	packetId = *reinterpret_cast<INT32*>(buf + sizeof(packetLength));
	return totalSize;
}

//PacketId Packet::GetPacketId()
//{
//	return PacketId::GAME_ENTER_REQ;
//	//return (PacketId)_packetId;
//}
