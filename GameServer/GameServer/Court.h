#pragma once
#include "Position.h"

enum class CourtArea
{
	OUT_OF_BOUNCE = 0,
	RED_AREA = 1,
	BLUE_AREA = 2
};

class Court
{
public:
	static CourtArea CheckAreaInCourt(Position pos);
private:
};

