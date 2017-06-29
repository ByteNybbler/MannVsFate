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

	enum class support_type
	{
		not,
		unlimited,
		limited
	};

	std::string name;
	std::string location;
	int total_count = 1;
	int max_active = 1;
	int spawn_count = 1;
	float wait_before_starting = 0.0f;
	float wait_between_spawns = 0.0f;
	support_type support = support_type::not;

	tfbot bot;
	tank tnk;
	// The type of entity spawned: TFBot or Tank.
	type type_of_spawned;

	// Variables for assistance with calculating pressure.

	float effective_pressure;

	// How long each spawn should theoretically take to kill, in seconds.
	float time_to_kill;
	
	float time_until_next_spawn = 0.0f;
	int spawns_remaining = 0;
	// How long until the time to kill expires.
	float time_to_kill_expires = 0.0f;

	// Ignore this WaveSpawn's effective pressure.
	//bool ignore_pressure = false;
};

#endif