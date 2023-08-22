#pragma once

class ControllReq : public Packet
{
public:
	INT64 controllTime;
	Acceleration contollAcc;
	ControllReq() : Packet(PacketId::CONTROLL_REQ) {}
	
	std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + sizeof(controllTime) + sizeof(contollAcc));

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc), reinterpret_cast<char*>(&contollAcc) + sizeof(contollAcc));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		std::copy(buf + offset, buf + sizeof(controllTime), &controllTime);
		offset += sizeof(controllTime);
		std::copy(buf + offset, buf + sizeof(contollAcc), reinterpret_cast<char*>(&contollAcc));
		offset += sizeof(contollAcc);
		return offset;
	}
};

class ControllNtf : public Packet
{
public:
	INT16 userIdx;
	INT64 controllTime;
	Acceleration contollAcc;
	ControllNtf() : Packet(PacketId::CONTROLL_NTF) {}

	std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + sizeof(userIdx) + sizeof(controllTime) + sizeof(contollAcc));

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&userIdx), reinterpret_cast<char*>(&userIdx) + sizeof(userIdx));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc), reinterpret_cast<char*>(&contollAcc) + sizeof(contollAcc));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		std::copy(buf + offset, buf + sizeof(userIdx), &userIdx);
		offset += sizeof(userIdx);
		std::copy(buf + offset, buf + sizeof(controllTime), &controllTime);
		offset += sizeof(controllTime);
		std::copy(buf + offset, buf + sizeof(contollAcc), reinterpret_cast<char*>(&contollAcc));
		offset += sizeof(contollAcc);
		return offset;
	}
};