// Generate random TFBots.

#ifndef BOT_GENERATOR_H
#define BOT_GENERATOR_H

#include "random_name_generator.h"
#include "tfbot.h"
#include "tfbot_meta.h"

class bot_generator
{
public:
	// Constructor.
	bot_generator();

	// Accessors.
	void set_possible_classes(const std::vector<player_class>& classes);
	void set_pressure_decay_rate(float in);

	// Generate a random TFBot.
	tfbot_meta generate_bot();

	// Make a bot into a giant.
	void make_bot_into_giant(tfbot_meta& bot_meta);

private:
	// The random name generator.
	const random_name_generator random_names;

	// The possible classes that the wave generator can choose from.
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
	float chanceMult;

	// The pressure decay rate.
	float pressure_decay_rate;
};

#endif