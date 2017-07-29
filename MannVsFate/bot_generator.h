// Generate random TFBots.

#ifndef BOT_GENERATOR_H
#define BOT_GENERATOR_H

#include "random_name_generator.h"
#include "tfbot.h"
#include "tfbot_meta.h"
#include "json_reader_weapon.h"

class pressure_manager;

class bot_generator
{
public:
	// Constructor.
	bot_generator(const pressure_manager& pm);

	// Accessors.
	void set_possible_classes(const std::vector<player_class>& classes);
	void set_giant_chance(float in);
	void set_boss_chance(float in);
	void set_giant_chance_increase(float in);
	void set_boss_chance_increase(float in);
	void set_engies_enabled(bool in);
	void set_scale_mega(float in);
	float get_scale_mega();
	void set_scale_doom(float in);
	float get_scale_doom();
	void set_generating_doombot(bool in);

	// Generate a random TFBot.
	tfbot_meta generate_bot();

	// Make a bot into a giant without modifying any stats unnecessarily.
	void make_bot_into_giant_pure(tfbot_meta& bot_meta);

	// Make a bot into a giant.
	void make_bot_into_giant(tfbot_meta& bot_meta);

	// Called whenever a wave ends.
	void wave_ended();

	// Add a bunch of weapon-specific attributes to a weapon.
	void randomize_weapon(weapon& wep, tfbot_meta& bot_meta);

	// Returns a muted pressure per player and mutes it more as the players gain more currency.
	float get_muted_damage_pressure(const float base) const;

	// Add cosmetics (hats etc) to a robot.
	void add_cosmetics(tfbot_meta& bot_meta);

private:
	// Injected dependencies.
	const pressure_manager& wave_pressure;

	// The weapon JSON reader is contained by this class so it only has to read the JSON data once.
	json_reader_weapon weapon_reader;

	// The random name generator.
	const random_name_generator random_names;
	// The possible classes that the bot generator can choose from.
	std::vector<player_class> possible_classes;
	// The multiplier on most random chance calls. A higher value here means more of the random chances will likely return true.
	float chance_mult;
	// The chance that a standard robot will end up being a giant.
	float giant_chance;
	// The chance that a giant will end up being a boss.
	float boss_chance;
	// How much giant_chance increases after each wave.
	float giant_chance_increase;
	// How much boss_chance increases after each wave.
	float boss_chance_increase;
	// Whether enemy engineers can spawn on the current map.
	bool engies_enabled;
	// Whether a doombot is being generated.
	bool generating_doombot;
	// The biggest scale that an enemy can possibly have when navigating through a map.
	// Does not take certain spawnbot locations into account.
	float scale_mega;
	// The scale of a doombot (doom robot). This is often too big to navigate all the way through a map.
	// This is fine though, since the doombots can't pick up flags.
	float scale_doom;
};

#endif