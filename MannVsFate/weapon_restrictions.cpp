#include "weapon_restrictions.h"

std::string weapon_restriction_to_string(const weapon_restrictions wr)
{
	switch (wr)
	{
	case weapon_restrictions::none:
		return "";
	case weapon_restrictions::primary:
		return "PrimaryOnly";
	case weapon_restrictions::secondary:
		return "SecondaryOnly";
	case weapon_restrictions::melee:
		return "MeleeOnly";
	default:
		throw std::exception("weapon_restriction_to_string exception: Invalid weapon_restrictions.");
	}
}