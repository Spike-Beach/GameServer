#pragma once
#include "ThreeValues.h"

class Acceleration : public ThreeValues
{
public:
	Acceleration() : ThreeValues() {}
	Acceleration(float setX, float setY, float setZ) : ThreeValues(setX, setY, setZ) {}
	Acceleration(float scalar) : ThreeValues(scalar) {}

	Acceleration& operator=(const Acceleration& other);
	Acceleration& operator=(const ThreeValues& tv);
	Acceleration operator+(const Acceleration& other) const;
	Acceleration operator-(const Acceleration& other) const;
	Acceleration operator*(const float& scalar) const;
	Acceleration& operator+=(const Acceleration& other);
	Acceleration& operator-=(const Acceleration& other);
	Acceleration GetNomalAcc();
	std::vector<char> Serialize();
	size_t Deserialize(char* buf, size_t len);
};

