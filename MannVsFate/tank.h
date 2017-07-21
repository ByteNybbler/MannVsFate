#ifndef TANK_H
#define TANK_H

#include "spawnable.h"

struct tank : public spawnable
{
	// Constructor.
	tank(int health, float speed)
		: health(health), speed(speed)
	{}

	type get_type() const
	{
		return spawnable::type::tank;
	}

	int health;
	float speed;
};

#endif