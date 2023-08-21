#pragma once
#include "ThreeValues.h"

class Acceleration : public ThreeValues
{
public:
	Acceleration() : ThreeValues() {}
	Acceleration(float setX, float setY, float setZ) : ThreeValues(setX, setY, setZ) {}

	Acceleration& operator=(const Acceleration& other);
	Acceleration operator+(const Acceleration& other) const;
	Acceleration operator-(const Acceleration& other) const;
	Acceleration& operator+=(const Acceleration& other);
	Acceleration& operator-=(const Acceleration& other);
	std::vector<char> Serialize();
	size_t Deserialize(char* buf, size_t len);
};

