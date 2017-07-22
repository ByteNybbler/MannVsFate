#include "virtual_spawn.h"

// Comment out the following line to disable console logging of virtual spawns.
//#define VIRTUAL_SPAWN_DEBUG

#ifdef VIRTUAL_SPAWN_DEBUG
#include <iostream>
#endif

virtual_spawn::virtual_spawn(int time_to_kill, float pressure_per_second, int currency, float pressure_per_second_per_second)
	: seconds_remaining(time_to_kill), pressure_per_second(pressure_per_second), currency(currency),
	pressure_per_second_per_second(pressure_per_second_per_second)
{}

virtual_spawn::virtual_spawn(int time_to_kill, float initial_pressure_per_second, float total_pressure, int currency)
	: seconds_remaining(time_to_kill), pressure_per_second(initial_pressure_per_second), currency(currency)
{
	// Note that pressure is not applied when 0 seconds remain of a virtual spawn, so we'll account for that.
	const int time_to_use = time_to_kill - 1;
	// Let's use the magic of kinematic equations.
	// a = 2(d-vt)/(t^2)
	pressure_per_second_per_second = 2 * (total_pressure - initial_pressure_per_second * time_to_use) / (time_to_use * time_to_use);
	// Since we are only measuring time in the intervals of whole numbers, we'll have a little bit of error.
	// Let's correct this error.
	// Sum of first n integers: n(n+1)/2
	const float errored_sum = pressure_per_second * time_to_use
		+ pressure_per_second_per_second * (time_to_use * (time_to_use - 1)) * 0.5f;
	// Apply the fixed error.
	pressure_per_second += (total_pressure - errored_sum) / time_to_use;

#ifdef VIRTUAL_SPAWN_DEBUG
	std::cout << "virtual_spawn total_pressure: " << total_pressure << std::endl;
	std::cout << "virtual_spawn errored_sum: " << errored_sum << std::endl;
	std::cout << "virtual_spawn time_to_kill: " << time_to_kill << std::endl;
	std::cout << "virtual_spawn initial pressure_per_second: " << pressure_per_second << std::endl;
	std::cout << "virtual_spawn pressure_per_second_per_second: " << pressure_per_second_per_second << std::endl;
	float new_sum = pressure_per_second * time_to_use
		+ pressure_per_second_per_second * (time_to_use * (time_to_use - 1)) * 0.5f;
	std::cout << "virtual_spawn new_sum: " << new_sum << std::endl;
	std::getchar();
#endif
}

void virtual_spawn::decrement_time()
{
	--seconds_remaining;
	pressure_per_second += pressure_per_second_per_second;
}

bool virtual_spawn::is_dead()
{
	return seconds_remaining <= 0;
}

int virtual_spawn::get_currency() const
{
	return currency;
}

float virtual_spawn::get_pressure_per_second() const
{
	return pressure_per_second;
}