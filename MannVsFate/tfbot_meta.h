// An extension of the tfbot class.

#ifndef TFBOT_META_H
#define TFBOT_META_H

#include "tfbot.h"

class tfbot_meta
{
public:
	// This is the multiplier for calculating the bot's threat level by multiplying its health.
	// The multiplier is adjusted based on a variety of factors.
	// A higher multiplier means the bot is harder to deal with.
	float pressure = 1.0f;

	// Access the bot.
	tfbot& get_bot();

	// Make this bot into a giant.
	void make_giant();

	// Update the bot's class icon based on the base class icon and the giants and crits statuses.
	void update_class_icon();

	// Set the bot's base class icon (excluding crits and giant). Also updates the wrapped bot's icon accordingly.
	void set_base_class_icon(const std::string& new_base_class_icon);

	std::string get_base_class_icon();

	// The move speed bonus of the bot.
	float move_speed_bonus = 1.0f;
	// The damage bonus of the bot.
	float damage_bonus = 1.0f;
	// Whether the bot is a giant.
	bool is_giant = false;
	// Whether the bot is a boss (i.e. unusually strong giant).
	bool is_boss = false;
	// Whether the bot is a doombot (i.e. enormous giant).
	bool is_doom = false;
	// Whether the bot cannot become a boss.
	bool perma_small = false;
	// Whether the bot has the AlwaysCrit attribute.
	bool is_always_crit = false;
	// Whether the bot has the AlwaysFireWeapon attribute.
	bool is_always_fire_weapon = false;

private:
	// The bot being wrapped.
	tfbot bot;
	// The base class icon of the bot, not including the giant or crits components.
	std::string base_class_icon = "";
};

#endif // TFBOT_META_H