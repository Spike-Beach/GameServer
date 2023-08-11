#pragma once
#include "pch.h"

class DisconnNtf : public Packet
{
public:
	DisconnNtf() : Packet(PacketId::CLOSE_CONN) {}
};

