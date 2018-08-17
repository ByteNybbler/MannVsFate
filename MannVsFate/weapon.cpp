#include "weapon.h"

// Set to 0 to disable debug messages for weapons.
#define WEAPON_DEBUG 0

#if WEAPON_DEBUG
#include <iostream>
#endif

weapon::weapon()
	: has_projectiles(false),
	arc_fire(false),
	can_be_switched_to(true),
	does_damage(true),
	bots_too_dumb_to_use(false),
	can_be_charged(false),
	burns(false),
	explodes(false),
	bleeds(false),
	has_effect_charge_bar(false),
	projectile_override_crash_risk(false)
{}

weapon::~weapon()
{
#if WEAPON_DEBUG
	std::cout << "Weapon destroyed: " << first_name() << std::endl;
#endif
}

bool weapon::is_a(const std::string& name)
{
	for (unsigned int i = 0; i < names.size(); ++i)
	{
		if (names.at(i) == name)
		{
			return true;
		}
	}
	return false;
}

std::string weapon::first_name() const
{
	return names.at(0);
}

bool weapon::matches_restriction(weapon_restrictions restriction)
{
	if (type == slot::primary && (restriction == weapon_restrictions::primary || restriction == weapon_restrictions::none))
	{
		return true;
	}
	if (type == slot::secondary && restriction == weapon_restrictions::secondary)
	{
		return true;
	}
	if (type == slot::melee && restriction == weapon_restrictions::melee)
	{
		return true;
	}
	return false;
}