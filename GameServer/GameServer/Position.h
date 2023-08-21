#pragma once
#include "ThreeValues.h"
#include "Velocity.h"

class Position : public ThreeValues
{
public:
	Position() : ThreeValues() {}
	Position(float setX, float setY, float setZ) : ThreeValues(setX, setY, setZ) {}
	
	Position& operator=(const Position& other);
	Position operator+(const Position& other) const;
	Position operator-(const Position& other) const;
	Position& operator+=(const Position& other);
	Position& operator-=(const Position& other);
	void CalPosition(Velocity vel, float elapsedSec);
	std::vector<char> Serialize();
	size_t Deserialize(char* buf, size_t len);
};


