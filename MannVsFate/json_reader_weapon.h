#ifndef JSON_READER_WEAPON_H
#define JSON_READER_WEAPON_H

#include "weapon.h"
#include "player_class.h"
#include "json.hpp"
#include <string>

// A JSON reader class for reading weapon data.
class json_reader_weapon
{
public:
	// Constructor.
	json_reader_weapon();

	// Read weapon data from the weapons JSON and return the corresponding weapon.
	weapon read_weapon(const std::string& weapon_name, const player_class cl) const;

private:
	// Reads a node of weapon data from the JSON.
	void read_weapon_node(weapon& wep, const std::string& weapon_name, const player_class cl) const;

	// Represents the root of the file.
	nlohmann::json weapons_json;
};

#endif // JSON_READER_WEAPON_H