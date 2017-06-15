#ifndef WAVESPAWN_H
#define WAVESPAWN_H

#include "tfbot.h"
#include "tank.h"
#include <string>

struct wavespawn
{
	enum class type
	{
		tfbot,
		tank
	};

	std::string name;
	int total_count = 1;
	int max_active = 1;
	int spawn_count = 1;
	float wait_before_starting = 0.0f;
	float wait_between_spawns = 0.0f;
	tfbot bot;
	tank tnk;
	type type_of_spawned;

	// Variables for assistance with calculating pressure.

	float effective_pressure;
	float time_to_kill;

	float time_until_next_spawn = 0.0f;
	int spawns_remaining = 0;
};

#endif