#include "player_class.h"
#include "rand_util.h"

/*
std::string classes[] = {
	"Scout",
	"Soldier",
	"Pyro",
	"Demoman",
	"Heavyweapons",
	"Engineer",
	"Medic",
	"Sniper",
	"Spy"
};
*/

std::string player_class_to_string(const player_class cl)
{
	switch (cl)
	{
	case player_class::scout:
		return "Scout";
	case player_class::soldier:
		return "Soldier";
	case player_class::pyro:
		return "Pyro";
	case player_class::demoman:
		return "Demoman";
	case player_class::heavyweapons:
		return "Heavyweapons";
	case player_class::engineer:
		return "Engineer";
	case player_class::medic:
		return "Medic";
	case player_class::sniper:
		return "Sniper";
	case player_class::spy:
		return "Spy";
	default:
		throw std::exception("player_class_to_string exception: Invalid player_class.");
	}
}

player_class get_random_class()
{
	int c = rand_int(0, 9);
	return static_cast<player_class>(c);
}

int get_class_default_health(const player_class cl)
{
	switch (cl)
	{
	case player_class::scout:
		return 125;
	case player_class::soldier:
		return 200;
	case player_class::pyro:
		return 175;
	case player_class::demoman:
		return 175;
	case player_class::heavyweapons:
		return 300;
	case player_class::engineer:
		return 125;
	case player_class::medic:
		return 150;
	case player_class::sniper:
		return 125;
	case player_class::spy:
		return 125;
	default:
		throw std::exception("get_class_default_health exception: Invalid player_class.");
	}
}

std::string get_class_icon(const player_class cl)
{
	if (cl == player_class::heavyweapons)
	{
		return "heavy";
	}
	else if (cl == player_class::demoman)
	{
		return "demo";
	}
	else
	{
		return player_class_to_string(cl);
	}
}