// Generate random TFBots.

#ifndef BOT_GENERATOR_H
#define BOT_GENERATOR_H

#include "random_name_generator.h"
#include "tfbot.h"
#include "tfbot_meta.h"

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

private:
	// Injected dependencies.
	const pressure_manager& wave_pressure;

	// The random name generator.
	const random_name_generator random_names;

	// The possible classes that the bot generator can choose from.
	std::vector<player_class> possible_classes = {
		player_class::scout,
		player_class::soldier,
		player_class::pyro,
		player_class::demoman,
		player_class::heavyweapons,
		player_class::engineer,
		player_class::medic,
		player_class::sniper,
		player_class::spy
	};

	// The multiplier on most random chance calls. A higher value here means more of the random chances will likely return true.
	float chance_mult;

	// Whether a doombot is being generated.
	bool generating_doombot = false;

	// The chance that a standard robot will end up being a giant.
	float giant_chance = 0.1f;
	// The chance that a giant will end up being a boss.
	float boss_chance = 0.15f;
	// Whether enemy engineers can spawn on the current map.
	bool engies_enabled = true;
	// The biggest scale that an enemy can possibly have when navigating through a map.
	// Does not take certain spawnbot locations into account.
	float scale_mega;
	// The scale of a doombot (doom robot). This is often too big to navigate all the way through a map.
	// This is fine though, since the doombots can't pick up flags.
	float scale_doom;
};

#endif