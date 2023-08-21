#pragma once

class ControllReq : public Packet
{
public:
	INT64 controllTime;
	//Acceleration contollVel;
	Acceleration contollAcc;
	Acceleration stopAcc;
	ControllReq() : Packet(PacketId::CONTROLL_REQ) {}
	
	std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + sizeof(controllTime) + sizeof(contollAcc) + sizeof(stopAcc));

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		//serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollVel), reinterpret_cast<char*>(&contollVel) + sizeof(contollVel));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc), reinterpret_cast<char*>(&contollAcc) + sizeof(contollAcc));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&stopAcc), reinterpret_cast<char*>(&stopAcc) + sizeof(stopAcc));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		std::copy(buf + offset, buf + sizeof(controllTime), &controllTime);
		offset += sizeof(controllTime);
		/*std::copy(buf + offset, buf + sizeof(contollVel), &contollVel);
		offset += sizeof(contollVel);*/
		std::copy(buf + offset, buf + sizeof(contollAcc), reinterpret_cast<char*>(&contollAcc));
		offset += sizeof(contollAcc);
		std::copy(buf + offset, buf + sizeof(stopAcc), reinterpret_cast<char*>(&stopAcc));
		offset += sizeof(stopAcc);
		return offset;
	}
};

class ControllNtf : public Packet
{
public:
	INT16 userIdx;
	INT64 controllTime;
	//Velocity contollVel;
	Acceleration contollAcc;
	Acceleration stopAcc;
	ControllNtf() : Packet(PacketId::CONTROLL_NTF) {}

	std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();

		serializeVec.reserve(PACKET_SIZE + sizeof(userIdx) + sizeof(controllTime) + sizeof(contollAcc) + sizeof(stopAcc));

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&userIdx), reinterpret_cast<char*>(&userIdx) + sizeof(userIdx));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		//serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollVel), reinterpret_cast<char*>(&contollVel) + sizeof(contollVel));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc), reinterpret_cast<char*>(&contollAcc) + sizeof(contollAcc));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&stopAcc), reinterpret_cast<char*>(&stopAcc) + sizeof(stopAcc));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		std::copy(buf + offset, buf + sizeof(userIdx), &userIdx);
		offset += sizeof(userIdx);
		std::copy(buf + offset, buf + sizeof(controllTime), &controllTime);
		offset += sizeof(controllTime);
		//std::copy(buf + offset, buf + sizeof(contollVel), &contollVel);
		//offset += sizeof(contollVel);
		std::copy(buf + offset, buf + sizeof(contollAcc), reinterpret_cast<char*>(&contollAcc));
		offset += sizeof(contollAcc);
		std::copy(buf + offset, buf + sizeof(stopAcc), reinterpret_cast<char*>(&stopAcc));
		offset += sizeof(stopAcc);
		return offset;
	}
};