#include "json_reader_weapon.h"
#include <fstream>

json_reader_weapon::json_reader_weapon()
{
	std::ifstream weapons_file("data/weapons.json");
	if (!weapons_file)
	{
		const std::string exstr = "json_reader_weapon::read_weapon exception: Couldn't find weapons file \"data/weapons.json\".";
		throw std::exception(exstr.c_str());
	}
	// Read the file data into weapons_json.
	try
	{
		weapons_file >> weapons_json;
	}
	catch (const std::exception&)
	{
		throw "weapons.json exception: JSON parse error.";
	}

	weapons_file.close();
}

weapon json_reader_weapon::read_weapon(const std::string& weapon_name, const player_class cl) const
{
	// Instantiate the actual container for the weapon data.
	weapon wep;

	read_weapon_node(wep, weapon_name, cl);

	return wep;
}

void json_reader_weapon::read_weapon_node(weapon& wep, const std::string& weapon_name, const player_class cl) const
{
	wep.names.emplace_back(weapon_name);

	// The node containing the data of the weapon itself.
	nlohmann::json weapon_node;

	try
	{
		weapon_node = weapons_json.at(weapon_name);
	}
	catch (const std::exception&)
	{
		const std::string exstr = "weapons.json exception: Couldn't find an entry for " + weapon_name + '!';
		throw std::exception(exstr.c_str());
	}

	try
	{
		// Try to find a parent weapon, if one exists.
		const std::string parent = weapon_node.at("parent").get<std::string>();

		// If we're still in here, then the parent does exist!
		// Let's recur deeper.
		read_weapon_node(wep, parent, cl);
	}
	catch (const std::exception&)
	{
		// No parent found.
	}

	// Let's try to populate the actual values of the weapon.

	try
	{
		const std::string type_string = weapon_node.at("type").get<std::string>();
		if (type_string == "primary")
		{
			wep.type = weapon::slot::primary;
		}
		else if (type_string == "secondary")
		{
			wep.type = weapon::slot::secondary;
		}
		else if (type_string == "melee")
		{
			wep.type = weapon::slot::melee;
		}
		else if (type_string == "building")
		{
			wep.type = weapon::slot::building;
		}
		else if (type_string == "pda2")
		{
			wep.type = weapon::slot::pda2;
		}
	}
	catch (const std::exception&)
	{
		// No type found.
	}

	try
	{
		wep.has_projectiles = weapon_node.at("has_projectiles").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.arc_fire = weapon_node.at("arc_fire").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.can_be_switched_to = weapon_node.at("can_be_switched_to").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.does_damage = weapon_node.at("does_damage").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.bots_too_dumb_to_use = weapon_node.at("bots_too_dumb_to_use").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.can_be_charged = weapon_node.at("can_be_charged").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.burns = weapon_node.at("burns").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.explodes = weapon_node.at("explodes").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.bleeds = weapon_node.at("bleeds").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.has_effect_charge_bar = weapon_node.at("has_effect_charge_bar").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		wep.projectile_override_crash_risk = weapon_node.at("projectile_override_crash_risk").get<bool>();
	}
	catch (const std::exception&) {}

	try
	{
		// A node containing several different types.
		nlohmann::json types_node;
		types_node = weapon_node.at("types");
		// Get the weapon slot based on the class name.
		const std::string class_name = player_class_to_string(cl);
		const std::string restriction = types_node.at(class_name).get<std::string>();
		if (restriction == "primary")
		{
			wep.type = weapon::slot::primary;
		}
		else if (restriction == "secondary")
		{
			wep.type = weapon::slot::secondary;
		}
		else if (restriction == "melee")
		{
			wep.type == weapon::slot::melee;
		}
	}
	catch (const std::exception&) {}
}