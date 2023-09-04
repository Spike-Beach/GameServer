#pragma once
#include "Packet.h"
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define POS_SIZE 3 * sizeof(float)
#define VEL_SIZE 3 * sizeof(float)
#define ACC_SIZE 3 * sizeof(float)
#define USER_SYNC_DATA_SIZE 5 * sizeof(INT64) + 4 * (POS_SIZE + VEL_SIZE + ACC_SIZE)

class SyncReq : public Packet
{
public:
	SyncReq() : Packet(PacketId::SYNC_REQ) {}
	INT64 syncReqTime;

	virtual std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + sizeof(syncReqTime);
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&syncReqTime), reinterpret_cast<char*>(&syncReqTime) + sizeof(syncReqTime));
		return serializeVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);
		syncReqTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(syncReqTime);
		return offset;
	}
};

class SyncRes : public Packet
{
public:
	SyncRes() : Packet(PacketId::SYNC_RES) {}
	INT64 syncTime;
	std::array<INT64, 4> latency;
	std::array<std::tuple<Position, Velocity, Acceleration>, 4> users;

	virtual std::vector<char> Serialize()
	{
		packetLength = PACKET_SIZE + USER_SYNC_DATA_SIZE;
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(packetLength);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&syncTime), reinterpret_cast<char*>(&syncTime) + sizeof(syncTime));
		for (size_t i = 0; i < latency.size(); i++)
		{
			serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&latency[i]), reinterpret_cast<char*>(&latency[i]) + sizeof(latency[i]));
		}

		for (size_t i = 0; i < 4; i++)
		{
			SerializeTuple(users[i], serializeVec);
		}

		return serializeVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		syncTime = *reinterpret_cast<INT64*>(buf + offset);
		offset += sizeof(syncTime);

		for (size_t i = 0; i < latency.size(); i++)
		{
			latency[i] = *reinterpret_cast<INT64*>(buf + offset);
			offset += sizeof(latency[i]);
		}

		for (size_t i = 0; i < 4; i++)
		{
			offset += DeserializeTuple(buf + offset, len - offset, users[i]);
		}

		return offset;
	}

	void SerializeTuple(std::tuple<Position, Velocity, Acceleration>& userData, std::vector<char>& dst)
	{
		std::vector<char> tempSerializeVec1(std::get<0>(userData).Serialize());
		dst.insert(dst.end(), tempSerializeVec1.begin(), tempSerializeVec1.end());
		std::vector<char> tempSerializeVec2(std::get<1>(userData).Serialize());
		dst.insert(dst.end(), tempSerializeVec2.begin(), tempSerializeVec2.end());
		std::vector<char> tempSerializeVec3(std::get<2>(userData).Serialize());
		dst.insert(dst.end(), tempSerializeVec3.begin(), tempSerializeVec3.end());
	}

	size_t DeserializeTuple(char* buf, size_t len, std::tuple<Position, Velocity, Acceleration>& dst)
	{
		size_t offset = 0;

		std::get<0>(dst).x = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<0>(dst).y = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<0>(dst).z = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<1>(dst).x = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<1>(dst).y = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<1>(dst).z = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<2>(dst).x = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<2>(dst).y = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);
		std::get<2>(dst).z = *reinterpret_cast<float*>(buf + offset);
		offset += sizeof(float);

		return offset;
	}
};


