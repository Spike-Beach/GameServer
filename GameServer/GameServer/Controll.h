#pragma once

#define ACC_SIZE 3 * sizeof(float)

class ControllReq : public Packet
{
public:
	INT64 controllTime;
	Acceleration contollAcc;
	ControllReq() : Packet(PacketId::CONTROLL_REQ) {}
	
	std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + sizeof(controllTime) + ACC_SIZE;
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.x), reinterpret_cast<char*>(&contollAcc.x) + sizeof(float));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.y), reinterpret_cast<char*>(&contollAcc.y) + sizeof(float));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.z), reinterpret_cast<char*>(&contollAcc.z) + sizeof(float));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		controllTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(INT64);
		contollAcc.x = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		contollAcc.y = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		contollAcc.z = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
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
		packetLength = PACKET_SIZE + sizeof(userIdx) + sizeof(controllTime) + ACC_SIZE;
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&userIdx), reinterpret_cast<char*>(&userIdx) + sizeof(userIdx));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&controllTime), reinterpret_cast<char*>(&controllTime) + sizeof(controllTime));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.x), reinterpret_cast<char*>(&contollAcc.x) + sizeof(float));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.y), reinterpret_cast<char*>(&contollAcc.y) + sizeof(float));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&contollAcc.z), reinterpret_cast<char*>(&contollAcc.z) + sizeof(float));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		userIdx = *reinterpret_cast<INT16*>(buf + offset);
		offset += sizeof(userIdx);
		controllTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(controllTime);
		contollAcc.x = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		contollAcc.y = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		contollAcc.z = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		return offset;
	}
};