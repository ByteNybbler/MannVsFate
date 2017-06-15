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

	// Get a reference to the bot.
	tfbot& get_bot();

	// Make this bot into a giant.
	void make_giant();

	// The move speed bonus of the bot.
	float move_speed_bonus = 1.0f;
	// The damage bonus of the bot.
	float damage_bonus = 1.0f;
	// Whether the bot is a giant.
	bool isGiant = false;
	// Whether the bot is a boss (i.e. ultra-powerful giant).
	bool isBoss = false;
	// Whether the bot cannot become a boss.
	bool permaSmall = false;
	// Whether the bot has the AlwaysCrit attribute.
	bool isAlwaysCrit = false;
	// Whether the bot has the AlwaysFireWeapon attribute.
	bool isAlwaysFireWeapon = false;

private:
	// The bot being wrapped.
	tfbot bot;
};

#endif // TFBOT_META_H