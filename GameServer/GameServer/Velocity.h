#pragma once
#include "ThreeValues.h"
#include "Acceleration.h"

class Velocity : public ThreeValues
{
public:
	Velocity() : ThreeValues() {}
	Velocity(ThreeValues threeVal) : ThreeValues(threeVal) {}
	Velocity(float setX, float setY, float setZ) : ThreeValues(setX, setY, setZ) {}
	Velocity(float scalar) : ThreeValues(scalar) {}
	Velocity& operator=(const Velocity& other);
	Velocity operator+(const Velocity& other) const;
	Velocity operator-(const Velocity& other) const;
	Velocity& operator+=(const Velocity& other);
	Velocity& operator-=(const Velocity& other);
	Velocity operator*(const float& scalar) const;
	Velocity operator/(const float& scalar) const;

	std::optional<Velocity> GetNomalVel();
	void ApplyBrakes(float decelerate, float elapsedSec);
	float CalMaxVelApprochElapseSec(Acceleration acc, float maxVelMagnitude);
	void AdjustToMaxMagnitude(float Magnitude);
	std::vector<char> Serialize();
	size_t Deserialize(char* buf, size_t len);
	static Velocity CalNewVelocity(Acceleration acc, float elapsedSec);
};

