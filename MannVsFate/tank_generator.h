#ifndef TANK_GENERATOR_H
#define TANK_GENERATOR_H

#include "tank.h"
#include <memory>

class pressure_manager;

class tank_generator
{
public:
	// Constructor.
	tank_generator(pressure_manager& wave_pressure);
	// Accessors.
	void set_max_tank_speed(float in);

	// Generate and return a tank.
	std::unique_ptr<tank> generate_tank(float time_left_in_wave);

private:
	// Injected dependencies.
	pressure_manager& wave_pressure;

	// The highest speed at which tanks can move while still reaching the hatch without falling into the hatch.
	float max_tank_speed;
};

#endif // TANK_GENERATOR_H