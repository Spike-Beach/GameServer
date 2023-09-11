#pragma once

class GameStartNtf : public Packet
{
public:
	INT64 gameStartTime;
	std::array<std::string, 4> nickNames;
	
	GameStartNtf() : Packet(PacketId::GAME_START_NTF) {}

	std::vector<char> Serialize()
	{
		Packet::packetLength = PACKET_SIZE + sizeof(gameStartTime);
		for (size_t i = 0; i < nickNames.size(); i++)
		{
			Packet::packetLength += nickNames[i].length() + 1;
		}
		std::vector<char> serialized = Packet::Serialize();
		serialized.reserve(Packet::packetLength);
		
		char* tempPtr = reinterpret_cast<char*>(&gameStartTime);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(gameStartTime));
		for (auto& nickName : nickNames)
		{
			serialized.insert(serialized.end(), nickName.begin(), nickName.end());
			serialized.push_back('\0');
		}

		return serialized;
	}
	size_t Deserialize(char* buf, size_t length)
	{
		size_t offset = Packet::Deserialize(buf, length);

		gameStartTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(gameStartTime);

		for (auto& nickName : nickNames)
		{
			nickName = buf + offset;
			offset += nickName.size() + 1;
		}

		return offset;
	}
};
