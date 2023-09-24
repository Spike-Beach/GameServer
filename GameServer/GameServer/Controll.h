#pragma once

class ControllReq : public Packet
{
public:
	float xCtl; // -1: 오른쪽 / 0: 이동 없음 / -1 : 왼쪽
	float yCtl; // -1: 앞쪽   / 0: 이동 없음 / -1 : 뒷쪽
	ControllReq() : Packet(PacketId::CONTROLL_REQ) {}
	
	std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + sizeof(xCtl) + sizeof(yCtl);
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&xCtl), reinterpret_cast<char*>(&xCtl) + sizeof(xCtl));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&yCtl), reinterpret_cast<char*>(&yCtl) + sizeof(yCtl));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		xCtl = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(xCtl);
		yCtl = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(yCtl);
		return offset;
	}
};

class ControllNtf : public Packet
{
public:
	INT16 userIdx;
	INT64 expectedSyncDuration;
	float xAppliedCtl; 
	float yAppliedCtl;
	ControllNtf() : Packet(PacketId::CONTROLL_NTF) {}

	std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + sizeof(userIdx) + sizeof(expectedSyncDuration) + sizeof(xAppliedCtl) + sizeof(yAppliedCtl);
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&userIdx), reinterpret_cast<char*>(&userIdx) + sizeof(userIdx));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&expectedSyncDuration), reinterpret_cast<char*>(&expectedSyncDuration) + sizeof(expectedSyncDuration));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&xAppliedCtl), reinterpret_cast<char*>(&xAppliedCtl) + sizeof(xAppliedCtl));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&yAppliedCtl), reinterpret_cast<char*>(&yAppliedCtl) + sizeof(yAppliedCtl));
		return serializeVec;
	}

	size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		userIdx = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(userIdx);
		expectedSyncDuration = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(expectedSyncDuration);
		xAppliedCtl = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(xAppliedCtl);
		yAppliedCtl = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(yAppliedCtl);
		return offset;
	}
};