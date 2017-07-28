#ifndef WEAPON_RESTRICTIONS_H
#define WEAPON_RESTRICTIONS_H

#include <string>

enum class weapon_restrictions
{
	none,
	primary,
	secondary,
	melee
};

std::string weapon_restriction_to_string(const weapon_restrictions wr);

#endif // WEAPON_RESTRICTIONS_H