#pragma once

#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

#define USER_SYNC_DATA_SIZE 5 * sizeof(INT64) + 4 * (sizeof(Position) + sizeof(Velocity) + sizeof(Acceleration))

class SyncReq : public Packet
{
public:
	SyncReq() : Packet(PacketId::SYNC_REQ) {}
};

class SyncRes : public Packet
{
public:
	SyncRes() : Packet(PacketId::SYNC_RES) {}
	INT64 milSec;
	std::array<INT64, 4> delay;
	std::array<std::tuple<Position, Velocity, Acceleration>, 4> users;
	//std::tuple<Position, Velocity, Acceleration> red1;
	//std::tuple<Position, Velocity, Acceleration> red2;
	//std::tuple<Position, Velocity, Acceleration> blue1;
	//std::tuple<Position, Velocity, Acceleration> blue2;

	virtual std::vector<char> Serialize()
	{
		std::vector<char> serializeVec = Packet::Serialize();
		serializeVec.reserve(PACKET_SIZE + USER_SYNC_DATA_SIZE);

		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&milSec), reinterpret_cast<char*>(&milSec) + sizeof(milSec));
		serializeVec.insert(serializeVec.end(), reinterpret_cast<char*>(&delay), reinterpret_cast<char*>(&delay) + sizeof(delay));

		//SerializeTuple(red1, serializeVec);
		//SerializeTuple(red2, serializeVec);
		//SerializeTuple(blue1, serializeVec);
		//SerializeTuple(blue2, serializeVec);
		for (size_t i = 0; i < 4; i++)
		{
			SerializeTuple(users[i], serializeVec);
		}

		return serializeVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = Packet::Deserialize(buf, len);

		std::copy(buf + offset, buf + sizeof(INT64), &milSec);
		offset += sizeof(INT64);
		std::copy(buf + offset, buf + sizeof(delay), reinterpret_cast<char*>(&delay));
		offset += sizeof(delay);
		
		//offset += DeserializeTuple(buf, len, red1);
		//offset += DeserializeTuple(buf, len, red2);
		//offset += DeserializeTuple(buf, len, blue1);
		//offset += DeserializeTuple(buf, len, blue2);
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



