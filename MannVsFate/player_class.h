#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include <string>

enum class player_class
{
	scout,
	soldier,
	pyro,
	demoman,
	heavyweapons,
	engineer,
	medic,
	sniper,
	spy
};

std::string player_class_to_string(const player_class cl);

player_class get_random_class();

// Returns the default base health of a given class.
int get_class_default_health(const player_class cl);

// Returns a class icon for MVM based on a given class.
std::string get_class_icon(const player_class cl);

#endif