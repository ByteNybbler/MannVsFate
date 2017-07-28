#ifndef WEAPON_H
#define WEAPON_H

#include "weapon_restrictions.h"
#include <string>
#include <vector>

// A structure representing a TF2 weapon.
struct weapon
{
	// The loadout slots that a weapon can appear in.
	enum class slot
	{
		primary,
		secondary,
		melee,
		building,
		pda2
	};

	// Constructor.
	weapon();
	// Destructor.
	~weapon();

	// A vector containing the names of the weapon itself and all of its parents.
	std::vector<std::string> names;
	// The name of the weapon.
	//std::string name;
	// The name of the weapon's most ancestral item class. If the weapon doesn't have a parent, the item class will be the same as the weapon's name.
	//std::string base_class;
	// The weapon's loadout slot.
	slot type;
	// Whether the weapon fires projectiles.
	bool has_projectiles;
	// Whether the weapon's fired projectiles have arc fire.
	bool arc_fire;
	// Whether the weapon is meant to be switched to.
	// This is false for items like The Chargin' Targe, which has passive effects rather than being switched to directly.
	bool can_be_switched_to;
	// Whether the weapon can actually do any damage.
	bool does_damage;
	// Whether bots are too dumb to use this weapon properly.
	bool bots_too_dumb_to_use;
	// Whether the weapon has a meter that can make use of the SpawnWithFullCharge attribute.
	bool can_be_charged;
	// Whether the weapon causes burn damage.
	bool burns;
	// Whether the weapon creates explosions.
	bool explodes;
	// Whether the weapon causes bleed damage.
	bool bleeds;
	// Whether the weapon has an effect charge bar.
	bool has_effect_charge_bar;

	// Returns true if the weapon or one of its parents has this name.
	bool is_a(const std::string& name);

	// Gets the least ancestral name of the weapon.
	std::string first_name() const;

	// Returns true if the weapon's slot matches a weapon restriction.
	bool matches_restriction(weapon_restrictions restriction);
};

#endif // WEAPON_H