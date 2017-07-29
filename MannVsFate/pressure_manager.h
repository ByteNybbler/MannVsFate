#ifndef PRESSURE_MANAGER_H
#define PRESSURE_MANAGER_H

#include "virtual_spawn.h"
#include "virtual_wavespawn.h"
#include <vector>

class currency_manager;

// A class for managing pressure during an MVM wave.
class pressure_manager
{
public:
	// Constructor.
	pressure_manager(currency_manager& cm);
	// Accessors.
	float get_pressure() const;
	float get_pressure_decay_rate() const;
	void set_players(int in);
	int get_players() const;
	void multiply_pressure_decay_rate_multiplier(float multiplier);
	void set_pressure_decay_rate_multiplier_in_time(float in);
	void set_bot_path_length(float in);
	// Set the pressure to 0.
	void reset_pressure();

	// Step through time starting at time t. This increments the variable t if time is passed.
	// This function contains the loop that actually modifies the pressure.
	void step_through_time(int& t);
	// Add a virtual WaveSpawn to the virtual WaveSpawns vector.
	// Add the corresponding virtual spawn as well.
	void add_virtual_wavespawn(const virtual_wavespawn& ws);
	// Calculate the pressure decay rate.
	void calculate_pressure_decay_rate();
	// Returns the pressure decay rate per player.
	float get_pressure_decay_rate_per_player() const;

private:
	// Add a virtual spawn to the virtual spawns vector based on a virtual WaveSpawn.
	// Do the pressure increase of the first spawn as well.
	void add_virtual_spawn(const virtual_wavespawn& ws);

	// Injected dependencies.
	currency_manager& mission_currency;

	// A vector of virtual enemies. Each enemy is individually spawned.
	// This is used for both TFBots and Tanks.
	std::vector<virtual_spawn> spawns;
	// A vector of virtual WaveSpawns.
	std::vector<virtual_wavespawn> wavespawns;

	// How much pressure is currently being placed on the theoretical players of the mission.
	// More pressure means the players are having a harder time.
	// We're assuming our players are disposing of the robot baddies over time,
	// so this pressure value should decay over time if left unattended.
	// If there's too much pressure, we should ease up on the players.
	// If there's too little pressure, we should go harder on the players.
	float pressure;

	// The more currency the players have, the more damage they'll be able to deal.
	// The more damage the players can deal, the quicker they'll be able to dispose of the enemies.
	// The number of players also affects how quickly the robots can be dealt with.
	// Let's have the pressure decay based on this quantity of currency multiplied by the number of players.
	// This decay rate is measured per second.
	// Since this decay rate essentially calculates how powerful and capable the RED team is,
	// it is also used to calculate how powerful the robots should be.
	float pressure_decay_rate;

	// How many players the mission is intended for.
	// The number of players serves as a multiplier for the pressure decay rate.
	int players;

	// The base pressure decay rate, measured per player, pre-multiplication.
	int base_pressure_decay_rate;

	// A flat multiplier for the pressure decay rate.
	float pressure_decay_rate_multiplier;

	// The amount by which the pressure decay rate is multiplied when stepping through time.
	// This is effectively the main variable that determines the pacing of the mission.
	float pressure_decay_rate_multiplier_in_time;

	// This is the relative length of the map's bot path.
	// This is used to help determine pressure decay.
	// mvm_bigrock is used as a basis, with a length of 1.0f.
	float bot_path_length;

	// This is the multiplier for the initial pressure per second of a TFBot.
	float pps_factor_tfbot;

	// This is the multiplier for the initial pressure per second of a Tank.
	float pps_factor_tank;
};

#endif PRESSURE_MANAGER_H