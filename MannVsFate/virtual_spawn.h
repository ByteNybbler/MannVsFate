#ifndef virtual_spawn_H
#define virtual_spawn_H

// Represents a simulated enemy or group of enemies spawned at the same time.
// Can represent both TFBots and Tanks.
class virtual_spawn
{
public:
	// Constructors.

	virtual_spawn(int time_to_kill, float pressure_per_second, int currency, float pressure_per_second_per_second = 0.0f);
	// Calculates the pressure_per_second_per_second automatically.
	virtual_spawn(int time_to_kill, float initial_pressure_per_second, float total_pressure, int currency);

	// Decrement the time and change the pressure per second (if applicable).
	void decrement_time();
	// Returns true if the virtual spawn is dead (i.e. there is no time remaining).
	bool is_dead();

	// Accessors.
	int get_currency() const;
	float get_pressure_per_second() const;

private:
	// How many seconds remain until the enemy dies.
	int seconds_remaining;
	// How much pressure is given per second.
	float pressure_per_second;
	// How much the pressure per second increases... per second.
	float pressure_per_second_per_second;
	// How much currency is contained by this enemy.
	int currency;
};

#endif