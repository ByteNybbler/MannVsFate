#include "tank_generator.h"
#include "rand_util.h"
#include "pressure_manager.h"

tank_generator::tank_generator(pressure_manager& wave_pressure)
	: wave_pressure(wave_pressure),
	max_tank_speed(500.0f)
{}

void tank_generator::set_max_tank_speed(float in)
{
	max_tank_speed = in;
}

std::unique_ptr<tank> tank_generator::generate_tank(float time_left_in_wave)
{
	float lower_bound = 20.0f;
	float upper_bound = 60.0f;
	const float speed_factor = sqrt(max_tank_speed / upper_bound);

	if (rand_chance(0.3f))
	{
		upper_bound *= speed_factor;
	}
	if (rand_chance(0.3f))
	{
		upper_bound *= speed_factor;
	}
	if (time_left_in_wave <= 20)
	{
		// If the wave is almost over, make the tank faster!
		lower_bound *= speed_factor;
		upper_bound *= speed_factor;
	}

	float speed = rand_float(lower_bound, upper_bound);
	// Cap the tank speed based on the map.
	if (speed > max_tank_speed)
	{
		speed = max_tank_speed;
	}

	// The faster the tank moves, the smaller its health should be to account for its speed.
	// Let's make the health both dependent on and inversely proportional to the speed.
	int health = static_cast<int>(wave_pressure.get_pressure_decay_rate() * 1500.0f / speed);
	// Round the tank health to the nearest 1000.
	health = static_cast<int>(std::ceil(static_cast<float>(health) / 1000) * 1000);

	// Instantiate the new tank.
	return std::make_unique<tank>(health, speed);
}