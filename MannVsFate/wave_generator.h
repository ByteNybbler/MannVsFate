#ifndef WAVE_GENERATOR_H
#define WAVE_GENERATOR_H

#include "tfbot.h"
#include "wavespawn.h"
#include "popfile_writer.h"
#include <string>

class currency_manager;
class pressure_manager;
class bot_generator;
class tank_generator;

class wave_generator
{
public:
	// Constructor.
	wave_generator(currency_manager& cm, pressure_manager& pm, bot_generator& botgen, tank_generator& tankgen);
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
	void set_force_tfbot_hp(int in);
	void set_force_tank_hp(int in);
	void set_force_tank_speed(float in);

	// Generate the mission. argc and argv are taken only to be printed in the mission file as debug info.
	void generate_mission(int argc = 1, char** argv = nullptr);

private:
	// The version of the wave generator.
	static const std::string version;

	// Injected dependencies.
	currency_manager& mission_currency;
	pressure_manager& wave_pressure;
	bot_generator& botgen;
	tank_generator& tankgen;

	// The popfile writer to use.
	popfile_writer writer;

	// The current wave being generated.
	int current_wave;

	// -=- The following are a bunch of settings that end up at the top of the mission file. -=-

	int respawn_wave_time;
	int event_popfile;
	bool fixed_respawn_wave_time;
	int add_sentry_buster_when_damage_dealt_exceeds;
	int add_sentry_buster_when_kill_count_exceeds;
	bool can_bots_attack_while_in_spawn_room;

	// -=- Tuning values. -=-

	// The way that currency is being distributed.
	//currency_mode current_currency_mode = currency_mode::per_wave;
	// Multiplier for the Sentry Buster cooldown.
	float sentry_buster_cooldown;
	// The name of the map to generate for.
	std::string map_name;
	// The name of the mission (excluding the map name).
	std::string mission_name;
	// The total number of waves.
	int waves;
	// The maximum number of wavespawns per wave.
	unsigned int max_wavespawns;
	// The maximum amount of time that a wave can last.
	int max_time;
	// The maximum number of unique icons per wave. There can be 23 icons before the HUD starts to look stupid without Support.
	// With Support, there can be 22(?) icons.
	unsigned int max_icons;
	// The chance that a WaveSpawn will be a tank WaveSpawn.
	float tank_chance;
	// The maximum number of seconds that a TFBot WaveSpawn can last.
	int max_tfbot_wavespawn_time;
	// The maximum number of seconds that a Tank WaveSpawn can last.
	int max_tank_wavespawn_time;
	// Bitfield for randomly generated Wavespawn warning sounds.
	// 0: Do not use randomized sounds at all.
	// 1: Use randomized sounds for wavespawns.
	// 2: Play a randomized sound every second.
	int use_wacky_sounds;
	// The ratio of voiceover sounds to standard sounds for the randomized sounds.
	float wacky_sound_vo_ratio;
	// Whether this mission is a Doombot mission.
	bool doombot_enabled;
	// If the pointed value exists, all TFBots will have this amount of HP.
	std::unique_ptr<int> force_tfbot_hp;
	// If the pointed value exists, all Tanks will have this amount of HP.
	std::unique_ptr<int> force_tank_hp;
	// If the pointed value exists, all Tanks will have this amount of speed.
	std::unique_ptr<float> force_tank_speed;
};

#endif