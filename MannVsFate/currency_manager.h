#ifndef CURRENCY_MANAGER_H
#define CURRENCY_MANAGER_H

#include "wavespawn.h"
#include <vector>

// Manages a mission's currency.
class currency_manager
{
public:
	// Constructor.
	currency_manager();

	// Accessors.
	void set_currency(int amount);
	int get_currency() const;
	void add_currency(int amount);
	float get_currency_pressure_multiplier() const;
	void set_currency_per_wave(int in);
	void set_currency_per_wave_spread(int in);
	void set_currency_per_wavespawn(int in);
	int get_currency_per_wavespawn() const;
	void set_currency_per_wavespawn_spread(int in);
	void set_currency_per_wavespawn_limit(int in);
	void add_approximated_additional_currency(int amount);

	// Reset some values to have a clean slate for a new wave.
	void prepare_for_new_wave();
	// Returns true if the currency per wavespawn has hit the limit for the wave.
	// Always returns false if the limit is 0.
	bool has_currency_per_wavespawn_hit_limit();
	// Add currency obtained from a WaveSpawn to the total currency. Returns the amount of currency added.
	int calculate_additional_currency_from_wavespawn();
	// Add the wave currency to the total currency and fix any roundoff error from the WaveSpawn currency.
	// This fix to the roundoff error will be reflected by modifying the final WaveSpawn of the mission.
	void add_currency_from_wave(std::vector<wavespawn>& wavespawns);
	// Returns the value of the current_currency * currency_pressure_multiplier.
	int get_multiplied_currency() const;
	// Returns the pressure value of the currency.
	float get_currency_pressure() const;

private:
	// How much currency the players currently have.
	int current_currency;
	// The amount by which currency is multiplied when used to increase pressure.
	float currency_pressure_multiplier;
	// The approximate amount of currency the players have collected in the current wave.
	// This only counts WaveSpawn currency.
	// We use this to fix roundoff error caused by integer division.
	int approximated_additional_currency;
	// All WaveSpawn currency accumulated so far during the current wave.
	int wavespawn_currency_so_far;
	// How much currency the players can earn per wave (not counting bonuses).
	int currency_per_wave;
	// The range of currency per wave in each direction.
	// For example, 2500 currency_per_wave with a currency_spread of 250 will yield between 2250 to 2750 currency per wave inclusive.
	int currency_per_wave_spread;
	// How much currency the players can earn per WaveSpawn.
	// This augments the amount of currency given by currency_per_wave.
	int currency_per_wavespawn;
	// Like currency_spread, but for currency_per_wavespawn.
	int currency_per_wavespawn_spread;
	// The wave will end when the total WaveSpawn currency from the current wave reaches this limit. 0 disables this.
	int currency_per_wavespawn_limit;
	// The exponent by which the currency pressure is raised.
	float currency_exponent;
};

#endif