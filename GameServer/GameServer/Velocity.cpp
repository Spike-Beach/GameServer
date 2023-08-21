#include "pch.h"
#include "Velocity.h"

Velocity& Velocity::operator=(const Velocity& other)
{
	ThreeValues::operator=(other);
	return *this;
}

Velocity Velocity::operator+(const Velocity& other) const
{
	Velocity temp(x + other.x, y + other.y, z + other.z);
	return temp;
}

Velocity Velocity::operator-(const Velocity& other) const
{
	Velocity temp(x - other.x, y - other.y, z - other.z);
	return temp;
}

Velocity& Velocity::operator+=(const Velocity& other)
{
	(*this) = (*this) + other;
	return *this;
}

Velocity& Velocity::operator-=(const Velocity& other)
{
	(*this) = (*this) - other;
	return *this;
}

void  Velocity::CalVelocity(Acceleration acc, float elapsedSec)
{
	x += acc.x * elapsedSec;
	y += acc.y * elapsedSec;
	z += acc.z * elapsedSec;
}

std::vector<char> Velocity::Serialize()
{
	return ThreeValues::Serialize();
}

size_t Velocity::Deserialize(char* buf, size_t len)
{
	return ThreeValues::Deserialize(buf, len);
}