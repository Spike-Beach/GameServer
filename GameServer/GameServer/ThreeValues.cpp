#include "pch.h"
#include "ThreeValues.h"

ThreeValues& ThreeValues::operator=(const ThreeValues& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

ThreeValues ThreeValues::operator+(const ThreeValues& other) const
{
	ThreeValues temp(x + other.x, y + other.y, z + other.z);
	return temp;
}

ThreeValues ThreeValues::operator-(const ThreeValues& other) const
{
	ThreeValues temp(x - other.x, y - other.y, z - other.z);
	return temp;
}

ThreeValues& ThreeValues::operator+=(const ThreeValues& other)
{
	(*this) = (*this) + other;
	return *this;
}

ThreeValues& ThreeValues::operator-=(const ThreeValues& other)
{
	(*this) = (*this) - other;
	return *this;
}

ThreeValues ThreeValues::operator/(const float& scalar) const
{
	ThreeValues temp;
	if (scalar == 0)
	{
		g_logger.Log(LogLevel::ERR, "ThreeValues::operator/", "Divide 0");
		throw std::exception("Divide 0");
	}
	temp.x /= scalar;
	temp.y /= scalar;
	temp.z /= scalar;
	return true;
}

ThreeValues ThreeValues::operator*(const float& scalar) const
{
	ThreeValues temp(x * scalar, y * scalar, z * scalar);
	return temp;
}

void ThreeValues::ScalarAdd(const float scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;
}

void ThreeValues::ScalarSub(const float scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;
}

void ThreeValues::ScalarMul(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

bool ThreeValues::ScalarDiv(const float scalar)
{
	if (scalar == 0)
	{
		g_logger.Log(LogLevel::ERR, "ThreeValues::ScalarDiv()", "scalar is 0");
		return false;
	}
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return true;
}

float ThreeValues::GetMagnitude()
{
	return std::sqrtf(std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2));
}

ThreeValues ThreeValues::GetNomal()
{
	ThreeValues temp(*this);
	float magnitude = std::sqrtf(std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2));
	if (magnitude > EPSILON)
	{
		temp.ScalarDiv(magnitude);
	}
	else
	{
		temp.ScalarMul(0);
	}
	return temp;
}

bool ThreeValues::IsZero() const
{
	return ((x < EPSILON && x > -EPSILON) && (y < EPSILON && y > -EPSILON) && (z < EPSILON && z > -EPSILON));
}

std::vector<char> ThreeValues::Serialize()
{
	std::vector<char> serializedVec;
	serializedVec.reserve(3 * sizeof(float));
	char* tempPtr = reinterpret_cast<char*>(&x);
	serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(float));
	tempPtr = reinterpret_cast<char*>(&y);
	serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(float));
	tempPtr = reinterpret_cast<char*>(&z);
	serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(float));

	return serializedVec;
}

size_t ThreeValues::Deserialize(char* buf, size_t len)
{
	size_t offset = 0;
	if (len < sizeof(float) * 3)
	{
		g_logger.Log(LogLevel::ERR, "ThreeValues::Deserialize()", "len is too small");
		return 0;
	}
	x = *reinterpret_cast<float*>(buf + offset);
	offset += sizeof(float);
	y = *reinterpret_cast<float*>(buf + offset);
	offset += sizeof(float);
	z = *reinterpret_cast<float*>(buf + offset);
	offset += sizeof(float);
	return offset;
}