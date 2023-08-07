#pragma once

//#include "IocpData.h" 순환참조

#pragma pack(push,1)
class Packet
{
public:
	INT32 packetLength;
	INT32 packetId;
	Packet(INT32 packetId) : packetId(packetId) {}
	virtual std::vector<char> Serialize(); //use std::copy
	virtual size_t Deserialize(char* buf, size_t length);
	//PacketId GetPacketId();
	static INT32 ParsePacketLength(char* base, size_t bufSize);
	static PacketId GetPacketId(char* base, size_t bufSize);
};

#pragma pack(pop)


