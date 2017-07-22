#ifndef VIRTUAL_WAVESPAWN_H
#define VIRTUAL_WAVESPAWN_H

// Represents a simulated WaveSpawn.
struct virtual_wavespawn
{
	// The effective pressure of the WaveSpawn.
	float effective_pressure;
	// How long each spawn should theoretically take to kill, in seconds.
	float time_to_kill;
	// How many seconds occur between spawns.
	float wait_between_spawns;
	// How many seconds remain until the next spawn occurs.
	float time_until_next_spawn;
	// How many spawns remain.
	int spawns_remaining;
	// How much currency is given per spawn.
	int currency_per_spawn;
	// Whether the WaveSpawn is for Tanks or not.
	bool is_tank;
};

#endif