#pragma once

class GameStartNtf : public Packet
{
public:
	INT64 gameStartTime;
	// userIdx : 0(red 1p), 1(red 2p), 2(blue 1p), 3(blue 2p)
	std::array<std::string, 4> nickNames;
	std::array<Position, 4> positions;
	
	GameStartNtf() : Packet(PacketId::GAME_START_NTF) {}

	std::vector<char> Serialize()
	{

		Packet::packetLength = PACKET_SIZE + sizeof(gameStartTime) + 4 * POSITION_SIZE;
		for (size_t i = 0; i < nickNames.size(); i++)
		{
			Packet::packetLength += nickNames[i].length() + 1;
		}

		std::vector<char> serialized = Packet::Serialize();
		serialized.reserve(Packet::packetLength);
		//g_logger.Log(LogLevel::INFO, "Reserved", "Reserved:" + std::to_string(Packet::packetLength));

		char* tempPtr = reinterpret_cast<char*>(&gameStartTime);
		serialized.insert(serialized.end(), tempPtr, tempPtr + sizeof(gameStartTime));
		for (auto nickName : nickNames)
		{
			serialized.insert(serialized.end(), nickName.begin(), nickName.end());
			serialized.push_back('\0');
			std::string tempSerializePos;
			tempSerializePos.insert(tempSerializePos.end(), serialized.end() - nickName.size(), serialized.end());
			//g_logger.Log(LogLevel::INFO, "Nick", "Nick:" + tempSerializePos);
			//g_logger.Log(LogLevel::INFO, "nickLeng", "Leng:" + std::to_string(serialized.size()));
		}

		for (auto posElem : positions)
		{
			std::vector<char> tempSerializePos(posElem.Serialize());
			/*g_logger.Log(LogLevel::INFO, "Pos", "PosX:" + std::to_string(*reinterpret_cast<float*>(&tempSerializePos[0])));
			g_logger.Log(LogLevel::INFO, "Pos", "PosY:" + std::to_string(*reinterpret_cast<float*>(&tempSerializePos[sizeof(float)])) );*/
			serialized.insert(serialized.end(), tempSerializePos.begin(), tempSerializePos.end());
			//g_logger.Log(LogLevel::INFO, "posLeng", "Leng:" + std::to_string(serialized.size()));
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

		for (int i = 0; i < positions.size(); i++)
		{
			positions[i].x = *reinterpret_cast<float*>(buf + offset);
			offset += sizeof(float);
			positions[i].y = *reinterpret_cast<float*>(buf + offset);
			offset += sizeof(float);
			positions[i].z = *reinterpret_cast<float*>(buf + offset);
			offset += sizeof(float);
		}

		return offset;
	}
};
