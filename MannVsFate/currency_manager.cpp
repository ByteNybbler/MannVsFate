#include "currency_manager.h"
#include "rand_util.h"

// Set to 0 to prevent debug messages about currency pressure.
#define CURRENCY_MANAGER_CURRENCY_PRESSURE_DEBUG 0
// Set to 0 to prevent debug messages about wavespawn currency.
#define CURRENCY_MANAGER_DEBUG_WAVESPAWN_CURRENCY 0

#if CURRENCY_MANAGER_CURRENCY_PRESSURE_DEBUG || CURRENCY_MANAGER_DEBUG_WAVESPAWN_CURRENCY
#include <iostream>
#endif

currency_manager::currency_manager()
	: current_currency(2000),
	currency_pressure_multiplier(0.04f),
	currency_exponent(1.5f),
	currency_pressure_multiplier_flat(1.0f), // 0.75f
	approximated_additional_currency(0),
	wavespawn_currency_so_far(0),
	currency_per_wave(1500),
	currency_per_wave_spread(0),
	currency_per_wavespawn(0),
	currency_per_wavespawn_spread(0),
	currency_per_wavespawn_limit(0)
{}

void currency_manager::set_currency(int amount)
{
	current_currency = amount;
}

int currency_manager::get_currency() const
{
	return current_currency;
}

void currency_manager::add_currency(int amount)
{
	current_currency += amount;
}

float currency_manager::get_currency_pressure_multiplier() const
{
	return currency_pressure_multiplier;
}

void currency_manager::set_currency_per_wave(int in)
{
	currency_per_wave = in;
}

void currency_manager::set_currency_per_wave_spread(int in)
{
	currency_per_wave_spread = in;
}

void currency_manager::set_currency_per_wavespawn(int in)
{
	currency_per_wavespawn = in;
}

int currency_manager::get_currency_per_wavespawn() const
{
	return currency_per_wavespawn;
}

void currency_manager::set_currency_per_wavespawn_spread(int in)
{
	currency_per_wavespawn_spread = in;
}

void currency_manager::set_currency_per_wavespawn_limit(int in)
{
	currency_per_wavespawn_limit = in;
}

void currency_manager::add_approximated_additional_currency(int amount)
{
	approximated_additional_currency += amount;
}

void currency_manager::prepare_for_new_wave()
{
	approximated_additional_currency = 0;
	wavespawn_currency_so_far = 0;
}

bool currency_manager::has_currency_per_wavespawn_hit_limit()
{
	const bool return_value = wavespawn_currency_so_far >= currency_per_wavespawn_limit && currency_per_wavespawn_limit != 0;
#if CURRENCY_MANAGER_DEBUG_WAVESPAWN_CURRENCY
	std::cout << "currency_manager wavespawn_currency_so_far: " << wavespawn_currency_so_far << std::endl;
	std::cout << "currency_manager has_currency_per_wavespawn_hit_limit: ";
	if (return_value)
	{
		std::cout << "true";
		std::getchar();
	}
	else
	{
		std::cout << "false";
	}
	std::cout << std::endl;
#endif
	return return_value;
}

int currency_manager::calculate_additional_currency_from_wavespawn()
{
	int additional_currency = currency_per_wavespawn + rand_int(-currency_per_wavespawn_spread, currency_per_wavespawn_spread);

	// Make sure this Wavespawn's currency doesn't go above the cap.
	if (wavespawn_currency_so_far + additional_currency > currency_per_wavespawn_limit)
	{
		additional_currency = currency_per_wavespawn_limit - wavespawn_currency_so_far;
	}

	wavespawn_currency_so_far += additional_currency;
	current_currency += additional_currency;
	return additional_currency;
}

void currency_manager::add_currency_from_wave(std::vector<wavespawn>& wavespawns)
{
	if (currency_per_wave != 0)
	{
		// Calculate the effective currency in this wave by factoring in the currency spread.
		int currency_in_this_wave = currency_per_wave + rand_int(-currency_per_wave_spread, currency_per_wave_spread);
		// Make sure the currency in this wave doesn't dip below 0.
		if (currency_in_this_wave < 0)
		{
			currency_in_this_wave = 0;
		}

		// Increment the total currency accrued so far (not counting bonuses).
		current_currency += currency_in_this_wave;

		// Calculate how much currency each wavespawn will have (after the fact).
		int currency_per_wavespawn = currency_in_this_wave / wavespawns.size();

		for (wavespawn& ws : wavespawns)
		{
			ws.total_currency += currency_per_wavespawn;
		}

		// Calculate the roundoff error from integer division.
		int approx_sum_currency_per_wavespawn = currency_per_wavespawn * wavespawns.size();
		int error = currency_per_wave - approx_sum_currency_per_wavespawn;

		// Correct this error by adding some extra currency to the last WaveSpawn.
		wavespawn& ws = wavespawns.back();
		ws.total_currency += error;
	}

	if (approximated_additional_currency != 0)
	{
		// Let's fix the roundoff error of integer division.
		current_currency -= approximated_additional_currency;
		if (currency_per_wavespawn_limit == wavespawn_currency_so_far)
		{
			current_currency += currency_per_wavespawn_limit;
		}
		else
		{
			int true_additional_currency = currency_per_wavespawn * wavespawns.size();
			current_currency += true_additional_currency;
		}
	}
}

int currency_manager::get_multiplied_currency() const
{
	return static_cast<int>(current_currency * currency_pressure_multiplier);
}

float currency_manager::get_currency_pressure() const
{
	const float return_value = powf(static_cast<float>(get_multiplied_currency()), currency_exponent)
		* currency_pressure_multiplier_flat;
#if CURRENCY_MANAGER_CURRENCY_PRESSURE_DEBUG
	std::cout << "currency_manager current_currency: " << current_currency << std::endl;
	std::cout << "currency_manager get_multiplied_currency: " << get_multiplied_currency() << std::endl;
	std::cout << "currency_manager get_currency_pressure return_value: " << return_value << std::endl;
	std::getchar();
#endif
	return return_value;
}