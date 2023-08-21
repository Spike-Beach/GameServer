#pragma once

class GameTimeoutNtf : public Packet
{
public:
	GameTimeoutNtf() : Packet(PacketId::GAME_TIMEOUT_NTF) {}
};
