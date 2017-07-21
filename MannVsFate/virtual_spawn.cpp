#include "virtual_spawn.h"

virtual_spawn::virtual_spawn(int time_to_kill, float pressure_per_second, int currency)
	: seconds_remaining(time_to_kill), pressure_per_second(pressure_per_second), currency(currency)
{}

void virtual_spawn::decrement_time()
{
	--seconds_remaining;
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