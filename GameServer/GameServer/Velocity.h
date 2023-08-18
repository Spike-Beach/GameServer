#pragma once
#include "Acceleration.h"

class Velocity : public ThreeValues
{
public:
	Velocity(coordType setX, coordType setY, coordType setZ) : ThreeValues(setX, setY, setZ) {}
	void  CalVelocity(Acceleration acc, float elapsedSec)
	{
		x += acc.x * elapsedSec;
		y += acc.y * elapsedSec;
		z += acc.z * elapsedSec;
	}
};

