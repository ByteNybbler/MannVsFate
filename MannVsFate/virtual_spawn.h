#ifndef virtual_spawn_H
#define virtual_spawn_H

// Represents a simulated enemy or group of enemies spawned at the same time.
// Can represent both TFBots and Tanks.
class virtual_spawn
{
public:
	// Constructor.
	virtual_spawn(int time_to_kill, float pressure_per_second, int currency);
	// Decrement the time.
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
	// How much currency is contained by this enemy.
	int currency;
};

#endif