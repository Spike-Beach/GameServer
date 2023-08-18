#pragma once
#include "Velocity.h"

class Position : public ThreeValues
{
	public:
	Position(coordType setX, coordType setY, coordType setZ) : ThreeValues(setX, setY, setZ) {}
	
	void  CalPosition(Velocity vel, float elapsedSec)
	{
		x += vel.x * elapsedSec;
		y += vel.y * elapsedSec;
		z += vel.z * elapsedSec;
	}
};


