// Generate random TFBots.

#ifndef BOT_GENERATOR_H
#define BOT_GENERATOR_H

#include "random_name_generator.h"
#include "tfbot.h"
#include "tfbot_meta.h"
#include "json_reader_weapon.h"

class pressure_manager;
class cosmetics_generator;

class bot_generator
{
public:
	// Constructor.
	bot_generator(const pressure_manager& pm, cosmetics_generator& cosgen);

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
	void set_give_bots_cosmetics(bool in);
	void set_fire_chance(float in);
	void set_bleed_chance(float in);
	void set_nonbosses_can_get_bleed(bool in);
	void set_bot_scale_chance(float in);
	void set_minimum_bot_scale(float in);
	void set_maximum_bot_scale(float in);
	void set_minimum_giant_scale(float in);
	void set_force_bot_scale(float in);

	// Generate a random TFBot.
	tfbot_meta generate_bot();

	// Perform a sanity check on the bot's scale.
	void check_bot_scale(tfbot_meta& bot_meta);

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

private:
	// Injected dependencies.
	const pressure_manager& wave_pressure;
	cosmetics_generator& cosmetics;

	// The weapon JSON reader is contained by this class so it only has to read the JSON data once.
	json_reader_weapon weapon_reader;
	// The item reader.
	list_reader item_reader;
	// The random name generator.
	random_name_generator random_names;
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
	// Whether to give the robots cosmetics.
	bool give_bots_cosmetics;
	// The chance that a weapon will get a fire buff.
	float fire_chance;
	// The chance that a weapon will get a bleed buff.
	float bleed_chance;
	// Whether non-boss robots can get bleed buff.
	bool nonbosses_can_get_bleed;
	// Chance to modify a robot's scale.
	float bot_scale_chance;
	// The minimum scale a non-giant bot can be.
	float minimum_bot_scale;
	// The maximum scale a non-giant bot can be.
	float maximum_bot_scale;
	// The minimum scale a giant robot can be. Overrides the minimum bot scale.
	float minimum_giant_scale;
	// If not negative, forces ALL bots to be this scale.
	float force_bot_scale;
};

#endif