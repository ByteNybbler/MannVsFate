#ifndef WAVE_GENERATOR_H
#define WAVE_GENERATOR_H

#include "tfbot.h"
#include "wavespawn.h"
#include "bot_generator.h"
#include "popfile_writer.h"
#include <string>

class wave_generator
{
	/*
	// How to distribute the currency.
	enum class currency_mode
	{
		per_wave,
		per_wavespawn
	};
	*/

public:
	// Accessors.
	void set_map_name(const std::string& in);
	void set_mission_name(const std::string& in);
	void set_starting_currency(int in);
	void set_waves(int in);
	void set_respawn_wave_time(int in);
	void set_event_popfile(int in);
	void set_fixed_respawn_wave_time(bool in);
	void set_add_sentry_buster_when_damage_dealt_exceeds(int in);
	void set_add_sentry_buster_when_kill_count_exceeds(int in);
	void set_can_bots_attack_while_in_spawn_room(bool in);
	void set_sentry_buster_cooldown(float in);
	void set_players(int in);
	void set_currency_per_wave(int in);
	void set_max_time(int in);
	void set_tank_chance(float in);
	void set_max_icons(int in);
	void set_max_wavespawns(int in);
	void set_possible_classes(const std::vector<player_class>& classes);
	void set_max_tfbot_wavespawn_time(int in);
	void set_pressure_decay_rate_multiplier_in_time(float in);
	void set_giant_chance(float in);
	void set_boss_chance(float in);
	void set_currency_spread(int in);
	void set_use_wacky_sounds(int in);
	void set_wacky_sound_vo_ratio(float in);
	void set_currency_per_wavespawn(int in);
	void set_currency_per_wavespawn_spread(int in);
	// Generate the mission. argc and argv are taken only to be printed in the mission file as debug info.
	void generate_mission(int argc = 1, char** argv = nullptr);

private:
	// The version of the wave generator.
	static const std::string version;

	// The bot generator to use.
	bot_generator botgen;
	// The popfile writer to use.
	popfile_writer writer;

	// The current wave being generated.
	int current_wave = 0;

	// -=- The following are a bunch of settings that end up at the top of the mission file. -=-

	int starting_currency = 2000;
	int respawn_wave_time = 2;
	int event_popfile = 0;
	bool fixed_respawn_wave_time = false;
	int add_sentry_buster_when_damage_dealt_exceeds = 3000;
	int add_sentry_buster_when_kill_count_exceeds = 15;
	bool can_bots_attack_while_in_spawn_room = false;

	// -=- Tuning values. -=-

	// The way that currency is being distributed.
	//currency_mode current_currency_mode = currency_mode::per_wave;
	// Multiplier for the Sentry Buster cooldown.
	float sentry_buster_cooldown = 1.0f; //35.0f;
	// The name of the map to generate for.
	std::string map_name = "mvm_bigrock";
	// The name of the mission (excluding the map name).
	std::string mission_name = "gen";
	// The total number of waves.
	int waves = 9; // 7;
	// How many players the mission is intended for.
	int players = 4;
	// The base pressure decay rate, measured per player, pre-multiplication.
	int base_pressure_decay_rate = 50;
	// The pressure decay rate multiplier.
	float pressure_decay_rate_multiplier = 0.025f; // 0.03f;
	// The amount by which the pressure decay rate is multiplied when stepping through time.
	// This is effectively the main variable that determines the difficulty of the mission.
	float pressure_decay_rate_multiplier_in_time = 3.0f;
	// The maximum number of wavespawns per wave.
	unsigned int max_wavespawns = 50;
	// The maximum amount of time that a wave can last.
	int max_time = 300;
	// The maximum number of unique icons per wave. There can be 23 icons before the HUD starts to look stupid.
	unsigned int max_icons = 23;
	// How much currency the players can earn per wave (not counting bonuses).
	int currency_per_wave = 1500;
	// The range of currency per wave in each direction.
	// For example, 2500 currency_per_wave with a currency_spread of 250 will yield between 2250 to 2750 currency per wave inclusive.
	int currency_spread = 0;
	// How much currency the players can earn per WaveSpawn.
	// This augments the amount of currency given by currency_per_wave.
	int currency_per_wavespawn = 0;
	// Like currency_spread, but for currency_per_wavespawn.
	int currency_per_wavespawn_spread = 0;
	// The amount by which currency is multiplied when used to increase pressure.
	float currency_pressure_multiplier = 0.8f;
	// The chance that a WaveSpawn will be a tank WaveSpawn.
	float tank_chance = 0.03f;
	// The maximum number of seconds that a TFBot WaveSpawn can last.
	int max_tfbot_wavespawn_time = 120;
	// Bitfield for randomly generated Wavespawn warning sounds.
	// 0: Do not use randomized sounds at all.
	// 1: Use randomized sounds for wavespawns.
	// 2: Play a randomized sound every second.
	int use_wacky_sounds = 0;
	// The ratio of voiceover sounds to standard sounds for the randomized sounds.
	float wacky_sound_vo_ratio = 0.1f;

	// Calculate the effective pressure decay rate.
	//void calculate_effective_pressure_decay_rate
	//(const float& pressure_decay_rate, float& effective_pressure_decay_rate, const std::vector<wavespawn>& wavespawns, const int& t);
};

#endif