#include "pch.h"
#include "Position.h"

Position& Position::operator=(const Position& other)
{
	ThreeValues::operator=(other);
	return *this;
}

Position Position::operator+(const Position& other) const
{
	Position temp(x + other.x, y + other.y, z + other.z);
	return temp;
}

Position Position::operator-(const Position& other) const
{
	Position temp(x - other.x, y - other.y, z - other.z);
	return temp;
}

Position& Position::operator+=(const Position& other)
{
	(*this) = (*this) + other;
	return *this;
}

Position& Position::operator-=(const Position& other)
{
	(*this) = (*this) - other;
	return *this;
}

void Position::CalPosition(Velocity vel, float elapsedSec)
{
	x += vel.x * elapsedSec;
	y += vel.y * elapsedSec;
	z += vel.z * elapsedSec;
}

std::vector<char> Position::Serialize()
{
	return ThreeValues::Serialize();
}

size_t Position::Deserialize(char* buf, size_t len)
{
	return ThreeValues::Deserialize(buf, len);
}