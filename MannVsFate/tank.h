#ifndef TANK_H
#define TANK_H

#include "spawnable.h"

struct tank : public spawnable
{
	// Constructor.
	tank(int health, float speed, int skin = 0)
		: health(health), speed(speed), skin(skin)
	{}

	type get_type() const
	{
		return spawnable::type::tank;
	}

	int health;
	float speed;
	int skin;
};

#endif