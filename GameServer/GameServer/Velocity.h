#pragma once
#include "ThreeValues.h"
#include "Acceleration.h"

class Velocity : public ThreeValues
{
public:
	Velocity() : ThreeValues() {}
	Velocity(float setX, float setY, float setZ) : ThreeValues(setX, setY, setZ) {}
	Velocity& operator=(const Velocity& other);
	Velocity operator+(const Velocity& other) const;
	Velocity operator-(const Velocity& other) const;
	Velocity& operator+=(const Velocity& other);
	Velocity& operator-=(const Velocity& other);

	void  CalVelocity(Acceleration acc, float elapsedSec);
	std::vector<char> Serialize();
	size_t Deserialize(char* buf, size_t len);
};

