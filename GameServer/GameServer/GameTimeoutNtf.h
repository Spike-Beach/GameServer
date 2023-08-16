#pragma once
#include "pch.h"

class GameTimeoutNtf : public Packet
{
public:
	GameTimeoutNtf() : Packet(PacketId::GAME_TIMEOUT_NTF) {}
};