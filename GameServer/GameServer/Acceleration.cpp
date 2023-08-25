#include "pch.h"
#include "Acceleration.h"

Acceleration& Acceleration::operator=(const Acceleration& other)
{
	ThreeValues::operator=(other);
	return *this;
}

Acceleration Acceleration::operator+(const Acceleration& other) const
{
	Acceleration temp(x + other.x, y + other.y, z + other.z);
	return temp;
}

Acceleration Acceleration::operator-(const Acceleration& other) const
{
	Acceleration temp(x - other.x, y - other.y, z - other.z);
	return temp;
}

Acceleration& Acceleration::operator+=(const Acceleration& other)
{
	(*this) = (*this) + other;
	return *this;
}

Acceleration& Acceleration::operator-=(const Acceleration& other)
{
	(*this) = (*this) - other;
	return *this;
}

Acceleration Acceleration::GetNomalAcc()
{
	Acceleration temp(*this);
	temp.ScalarDiv(std::sqrtf(std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2)));
	return temp;
}

std::vector<char> Acceleration::Serialize()
{
	return ThreeValues::Serialize();
}

size_t Acceleration::Deserialize(char* buf, size_t len)
{
	return ThreeValues::Deserialize(buf, len);
}