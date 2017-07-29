#include "tfbot_meta.h"
#include "json_reader_weapon.h"

tfbot_meta::tfbot_meta()
	: pressure(1.0f),
	move_speed_bonus(1.0f),
	damage_bonus(1.0f),
	is_giant(false),
	is_boss(false),
	is_doom(false),
	perma_small(false),
	is_always_crit(false),
	is_always_fire_weapon(false),
	base_class_icon(""),
	projectile_override_crash_risk(false),
	shall_be_giant(false),
	shall_be_boss(false)
{
	// Since we're keeping references to elements in the vector, we don't want the vector to move around.
	// Let's reserve as much space as we need.
	weapons.reserve(10);
}

tfbot& tfbot_meta::get_bot()
{
	return bot;
}

void tfbot_meta::make_giant()
{
	// The MiniBoss Attribute makes the bot giant.
	bot.attributes.emplace("MiniBoss");
	// The name of the footstep sound attribute.
	const std::string foot = "override footstep sound set";

	switch (bot.cl)
	{
	case player_class::scout:
		bot.character_attributes[foot] = 5;
		break;
	case player_class::soldier:
		bot.character_attributes[foot] = 3;
		break;
	case player_class::pyro:
		bot.character_attributes[foot] = 6;
		break;
	case player_class::demoman:
		bot.character_attributes[foot] = 4;
		break;
	case player_class::heavyweapons:
		bot.character_attributes[foot] = 2;
		break;
	case player_class::engineer:
		bot.character_attributes[foot] = 4;
		break;
	case player_class::medic:
		//bot.character_attributes[foot] = 5;
		break;
	case player_class::sniper:
		bot.character_attributes[foot] = 4;
		break;
	case player_class::spy:
		bot.character_attributes[foot] = 4;
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
	return pressure * bot.health;
}

float tfbot_meta::calculate_muted_pressure() const
{
	constexpr float mute_factor = 0.3f;
	return ((pressure - 1.0f) * mute_factor) + 1.0f;
}

float tfbot_meta::calculate_muted_effective_pressure() const
{
	return calculate_muted_pressure() * bot.health;
}

float tfbot_meta::calculate_time_to_kill(float recip_pressure_decay_rate) const
{
	return calculate_effective_pressure() * recip_pressure_decay_rate;
}

float tfbot_meta::calculate_muted_time_to_kill(float recip_pressure_decay_rate) const
{
	return calculate_muted_effective_pressure() * recip_pressure_decay_rate;
}

weapon& tfbot_meta::add_weapon(const std::string& weapon_name, const json_reader_weapon& weapon_reader)
{
	// Read the weapon from JSON data.
	weapon wep = weapon_reader.read_weapon(weapon_name, bot.cl);

	// Add the weapon to the weapons collection.
	weapons.emplace_back(wep);
	//std::cout << "tfbot_meta::add_weapon: Weapon names count: " << wep.names.size() << std::endl;

	// Give the bot the actual name of the item.
	bot.items.emplace(wep.names.at(0));

	// Return the most recent weapon (i.e. the one that was just added).
	weapon& ref = weapons.at(weapons.size() - 1);
	//std::cout << "tfbot_meta::add_weapon: added weapon: " << ref.first_name() << std::endl;
	//std::cout << "tfbot_meta::add_weapon: ref names count: " << ref.names.size() << std::endl;
	return ref;
	//slot = &weapons.at(weapons.size() - 1);
}