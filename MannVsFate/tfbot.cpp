#include "tfbot.h"

void tfbot::make_giant()
{
	// The MiniBoss Attribute makes the bot giant.
	attributes.emplace_back("MiniBoss");
	// The name of the footstep sound attribute.
	const std::string foot = "override footstep sound set";

	switch (cl)
	{
	case player_class::scout:
		character_attributes.emplace_back(foot, 5);
		break;
	case player_class::soldier:
		character_attributes.emplace_back(foot, 3);
		break;
	case player_class::pyro:
		character_attributes.emplace_back(foot, 6);
		break;
	case player_class::demoman:
		character_attributes.emplace_back(foot, 4);
		break;
	case player_class::heavyweapons:
		character_attributes.emplace_back(foot, 2);
		break;
	case player_class::engineer:
		character_attributes.emplace_back(foot, 4);
		break;
	case player_class::medic:
		//character_attributes.emplace_back(foot, 5);
		break;
	case player_class::sniper:
		character_attributes.emplace_back(foot, 4);
		break;
	case player_class::spy:
		character_attributes.emplace_back(foot, 4);
		break;
	default:
		throw std::exception("tfbot::make_giant exception: Invalid player_class.");
	}
}