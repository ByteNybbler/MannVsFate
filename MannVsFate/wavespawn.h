#ifndef WAVESPAWN_H
#define WAVESPAWN_H

#include "spawnable.h"
#include <string>
#include <memory>

// A structure representing a WaveSpawn.
struct wavespawn
{
	// Copy constructor.
	// Required due to this class' use of std::unique_ptr.
	wavespawn(wavespawn& ws)
		: name(ws.name),
		location(ws.location),
		total_count(ws.total_count),
		max_active(ws.max_active),
		spawn_count(ws.spawn_count),
		wait_before_starting(ws.wait_before_starting),
		wait_between_spawns(ws.wait_between_spawns),
		support(ws.support),
		first_spawn_warning_sound(ws.first_spawn_warning_sound),
		total_currency(ws.total_currency),
		enemy(std::move(ws.enemy))
	{}

	// Default constructor.
	wavespawn()
	{}

	enum class support_type
	{
		not_support,
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
	support_type support = support_type::not_support;
	std::string first_spawn_warning_sound = "";
	int total_currency = 0;

	// The enemy contained by the WaveSpawn.
	std::unique_ptr<spawnable> enemy;
};

#endif