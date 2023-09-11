#include "pch.h"
#include "Court.h"

CourtArea Court::CheckAreaInCourt(Position pos)
{
	/*                    
	*              outside3
	*        [corner3] -- [corner4]
	*            |            |
	*            |            |
	* outside4 [net1] ----- [net2]  outside2
	*            |            |
	*            |            |
	*        [corner1] -- [corner2]
	*              outside1
	*/   
	static Position corner1(1591.234985f, 14.57222f, 33.601303f);
	static Position corner2(2389.548096f, 16.208084f, 33.747204f);
	static Position corner3(1591.234741f, 1613.24646f, 33.601303f);
	static Position corner4(2389.547852f, 1611.610596f, 33.747204f);
	static Position net1(1591.234863f, 813.90934f, 0);
	static Position net2(2389.547974f, 813.90934f, 0);

	bool outside1 = ((pos.y - corner1.y) * (corner2.x - corner1.x) - (corner2.y - corner1.y) * (pos.x - corner1.x)) < 0;
	if (outside1 == true)
	{
		return CourtArea::OUT_OF_BOUNCE;
	}

	bool outside2 = ((pos.y - corner2.y) * (corner4.x - corner2.x) - (corner4.y - corner2.y) * (pos.x - corner2.x)) > 0;
	if (outside2 == true)
	{
		return CourtArea::OUT_OF_BOUNCE;
	}

	bool outside3 = ((pos.y - corner4.y) * (corner3.x - corner4.x) - (corner3.y - corner4.y) * (pos.x - corner4.x)) > 0;
	if (outside3 == true)
	{
		return CourtArea::OUT_OF_BOUNCE;
	}

	bool outside4 = ((pos.y - corner3.y) * (corner1.x - corner3.x) - (corner1.y - corner3.y) * (pos.x - corner3.x)) < 0;
	if (outside4 == true)
	{
		return CourtArea::OUT_OF_BOUNCE;
	}

	if (((pos.y - net1.y) * (net2.x - net1.x) - (net2.y - net1.y) * (pos.x - net1.x)) < 0)
	{
		return CourtArea::RED_AREA;
	}
	else
	{
		return CourtArea::BLUE_AREA;
	}
}