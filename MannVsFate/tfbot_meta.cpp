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

void tfbot_meta::update_class_icon()
{
	bot.class_icon = base_class_icon;
	if (is_always_crit)
	{
		bot.class_icon += "_crit";
	}
	if (is_giant)
	{
		bot.class_icon += "_giant";
	}
}

void tfbot_meta::set_base_class_icon(const std::string& new_base_class_icon)
{
	base_class_icon = new_base_class_icon;
	update_class_icon();
}

std::string tfbot_meta::get_base_class_icon() const
{
	return base_class_icon;
}

float tfbot_meta::calculate_effective_pressure() const
{
	//float muted_pressure = ((pressure - 1.0f) * 0.5f) + 1.0f;
	return pressure * bot.health;
}

float tfbot_meta::calculate_time_to_kill(float recip_pressure_decay_rate) const
{
	return calculate_effective_pressure() * recip_pressure_decay_rate;
}