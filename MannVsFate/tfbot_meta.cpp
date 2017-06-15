#include "tfbot_meta.h"

tfbot& tfbot_meta::get_bot()
{
	return bot;
}

void tfbot_meta::make_giant()
{
	// The MiniBoss Attribute makes the bot giant.
	bot.attributes.emplace_back("MiniBoss");
	// The name of the footstep sound attribute.
	const std::string foot = "override footstep sound set";

	switch (bot.cl)
	{
	case player_class::scout:
		bot.character_attributes.emplace_back(foot, 5);
		break;
	case player_class::soldier:
		bot.character_attributes.emplace_back(foot, 3);
		break;
	case player_class::pyro:
		bot.character_attributes.emplace_back(foot, 6);
		break;
	case player_class::demoman:
		bot.character_attributes.emplace_back(foot, 4);
		break;
	case player_class::heavyweapons:
		bot.character_attributes.emplace_back(foot, 2);
		break;
	case player_class::engineer:
		bot.character_attributes.emplace_back(foot, 4);
		break;
	case player_class::medic:
		//bot.character_attributes.emplace_back(foot, 5);
		break;
	case player_class::sniper:
		bot.character_attributes.emplace_back(foot, 4);
		break;
	case player_class::spy:
		bot.character_attributes.emplace_back(foot, 4);
		break;
	default:
		throw std::exception("tfbot_meta::make_giant exception: Invalid player_class.");
	}
}