#ifndef MISSION_H
#define MISSION_H

#include "tfbot.h"

struct mission
{
	std::string location = "";
	std::string objective = "";
	float initial_cooldown = 0.0f;
	float cooldown_time = 0.0f;
	int begin_at_wave = 1;
	int run_for_this_many_waves = 9999;
	int desired_count = 1;

	tfbot bot;
};

#endif // MISSION_H