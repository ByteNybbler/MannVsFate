#ifndef WAVE_GENERATOR_H
#define WAVE_GENERATOR_H

#include "tfbot.h"
#include "wavespawn.h"
#include "popfile_writer.h"
#include <string>

class currency_manager;
class pressure_manager;
class bot_generator;

class wave_generator
{
public:
	// Constructor.
	wave_generator(currency_manager& cm, pressure_manager& pm, bot_generator& botgen);
	// Accessors.
	void set_map_name(const std::string& in);
	void set_mission_name(const std::string& in);
	void set_waves(int in);
	void set_respawn_wave_time(int in);
	void set_event_popfile(int in);
	void set_fixed_respawn_wave_time(bool in);
	void set_add_sentry_buster_when_damage_dealt_exceeds(int in);
	void set_add_sentry_buster_when_kill_count_exceeds(int in);
	void set_can_bots_attack_while_in_spawn_room(bool in);
	void set_sentry_buster_cooldown(float in);
	void set_max_time(int in);
	void set_tank_chance(float in);
	void set_max_icons(int in);
	void set_max_wavespawns(int in);
	void set_max_tfbot_wavespawn_time(int in);
	void set_max_tank_wavespawn_time(int in);
	void set_use_wacky_sounds(int in);
	void set_wacky_sound_vo_ratio(float in);
	void set_doombot_enabled(bool in);

	// Generate the mission. argc and argv are taken only to be printed in the mission file as debug info.
	void generate_mission(int argc = 1, char** argv = nullptr);

private:
	// The version of the wave generator.
	static const std::string version;

	// Injected dependencies.
	currency_manager& mission_currency;
	pressure_manager& wave_pressure;
	bot_generator& botgen;

	// The popfile writer to use.
	popfile_writer writer;

	// The current wave being generated.
	int current_wave = 0;

	// -=- The following are a bunch of settings that end up at the top of the mission file. -=-

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
	// The maximum number of wavespawns per wave.
	unsigned int max_wavespawns = 0;
	// The maximum amount of time that a wave can last.
	int max_time = 300;
	// The maximum number of unique icons per wave. There can be 23 icons before the HUD starts to look stupid.
	unsigned int max_icons = 23;
	// The chance that a WaveSpawn will be a tank WaveSpawn.
	float tank_chance = 0.04f;
	// The maximum number of seconds that a TFBot WaveSpawn can last.
	int max_tfbot_wavespawn_time = 120;
	// The maximum number of seconds that a Tank WaveSpawn can last.
	int max_tank_wavespawn_time = 300;
	// Bitfield for randomly generated Wavespawn warning sounds.
	// 0: Do not use randomized sounds at all.
	// 1: Use randomized sounds for wavespawns.
	// 2: Play a randomized sound every second.
	int use_wacky_sounds = 0;
	// The ratio of voiceover sounds to standard sounds for the randomized sounds.
	float wacky_sound_vo_ratio = 0.1f;
	// Whether this mission is a Doombot mission.
	bool doombot_enabled = false;
};

#endif