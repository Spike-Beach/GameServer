#pragma once
#include "pch.h"

class CloseConnPacket : public Packet
{
public:
	CloseConnPacket() : Packet(PacketId::CLOSE_CONN) {}
};

