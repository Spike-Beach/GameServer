#pragma once

typedef float coordType;

class ThreeValues
{
public:
	coordType x;
	coordType y;
	coordType z;
	ThreeValues(coordType setX, coordType setY, coordType setZ) : x(setX), y(setY), z(setZ) {}

	ThreeValues& operator=(const ThreeValues& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	ThreeValues operator+(const ThreeValues& other)
	{
		ThreeValues temp(x + other.x, y + other.y, z + other.z);
		return temp;
	}

	ThreeValues operator-(const ThreeValues& other)
	{
		ThreeValues temp(x - other.x, y - other.y, z - other.z);
		return temp;
	}

	ThreeValues& operator+=(const ThreeValues& other)
	{
		(*this) = (*this) + other;
		return *this;
	}

	ThreeValues& operator-=(const ThreeValues& other)
	{
		(*this) = (*this) - other;
		return *this;
	}

	virtual void ScalarAdd(const coordType scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
	}

	virtual void ScalarSub(const coordType scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
	}

	virtual void ScalarMul(const coordType scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

	virtual bool ScalarDiv(const coordType scalar)
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

	virtual std::vector<char> Serialize()
	{
		std::vector<char> serializedVec;
		serializedVec.reserve(3 * sizeof(coordType));
		char* tempPtr = reinterpret_cast<char*>(&x);
		serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(coordType));
		tempPtr = reinterpret_cast<char*>(&y);
		serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(coordType));
		tempPtr = reinterpret_cast<char*>(&z);
		serializedVec.insert(serializedVec.end(), tempPtr, tempPtr + sizeof(coordType));
		return serializedVec;
	}

	virtual size_t Deserialize(char* buf, size_t len)
	{
		size_t offset = 0;
		if (len < sizeof(coordType) * 3)
		{
			g_logger.Log(LogLevel::ERR, "ThreeValues::Deserialize()", "len is too small");
			return;
		}
		x = *reinterpret_cast<coordType*>(buf + offset);
		offset += sizeof(coordType);
		y = *reinterpret_cast<coordType*>(buf + offset);
		offset += sizeof(coordType);
		z = *reinterpret_cast<coordType*>(buf + offset);
		offset += sizeof(coordType);
		return offset;
	}
};

