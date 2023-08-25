#pragma once

#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define USER_SYNC_DATA_SIZE 5 * sizeof(INT64) + 4 * (sizeof(Position) + sizeof(Velocity) + sizeof(Acceleration))

class SyncReq : public Packet
{
public:
	SyncReq() : Packet(PacketId::SYNC_REQ) {}
	INT64 mSec;

	virtual std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + sizeof(mSec));

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&mSec), reinterpret_cast<char*>(&mSec) + sizeof(mSec));
		return serializeVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		std::copy(buf + offset, buf + sizeof(mSec), &mSec);
		offset += sizeof(mSec);
		return offset;
	}
};

class SyncRes : public Packet
{
public:
	SyncRes() : Packet(PacketId::SYNC_RES) {}
	INT64 mSec;
	std::array<INT64, 4> latency;
	std::array<std::tuple<Position, Velocity, Acceleration>, 4> users;

	virtual std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + USER_SYNC_DATA_SIZE);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&mSec), reinterpret_cast<char*>(&mSec) + sizeof(mSec));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&latency), reinterpret_cast<char*>(&latency) + sizeof(latency));

		for (size_t i = 0; i < 4; i++)
		{
			SerializeTuple(users[i], serializeVec);
		}

		return serializeVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		std::copy(buf + offset, buf + sizeof(mSec), &mSec);
		offset += sizeof(mSec);
		std::copy(buf + offset, buf + sizeof(latency), reinterpret_cast<char*>(&latency));
		offset += sizeof(latency);
		
		for (size_t i = 0; i < 4; i++)
		{
			offset += DeserializeTuple(buf, len, users[i]);
		}

		return offset;
	}

	void SerializeTuple(std::tuple<Position, Velocity, Acceleration>& userData, std::vector<char>& dst)
	{
		std::vector<char> tempSerializeVec1(std::get<0>(userData).Serialize());
		dst.insert(tempSerializeVec1.begin(), tempSerializeVec1.size());
		std::vector<char> tempSerializeVec2(std::get<1>(userData).Serialize());
		dst.insert(tempSerializeVec2.begin(), tempSerializeVec2.size());
		std::vector<char> tempSerializeVec3(std::get<2>(userData).Serialize());
		dst.insert(tempSerializeVec3.begin(), tempSerializeVec3.size());
	}

	size_t DeserializeTuple(char* buf, size_t len, std::tuple<Position, Velocity, Acceleration>& dst)
	{
		size_t offset = 0;

		std::copy(buf, buf + sizeof(Position), reinterpret_cast<char*>(&std::get<0>(dst)));
		offset += sizeof(Position);
		std::copy(buf + offset, buf + sizeof(Velocity), reinterpret_cast<char*>(&std::get<1>(dst)));
		offset += sizeof(Velocity);
		std::copy(buf + offset, buf + sizeof(Acceleration), reinterpret_cast<char*>(&std::get<1>(dst)));
		offset += sizeof(Acceleration);
		return offset;
	}
};



