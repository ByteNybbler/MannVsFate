#ifndef TFBOT_H
#define TFBOT_H

#include "player_class.h"
#include "spawnable.h"
#include "weapon_restrictions.h"
#include <string>
#include <unordered_set>
#include <map>

struct tfbot : public spawnable
{
	type get_type() const
	{
		return spawnable::type::tfbot;
	}

	player_class cl;
	std::string class_icon;
	int health;
	float scale = -1.0f;
	std::string name = "ROBOT!!!";
	std::string skill = "Easy";
	weapon_restrictions weapon_restriction = weapon_restrictions::none;
	float max_vision_range = -1.0f;
	std::unordered_set<std::string> attributes;
	std::map<std::string, float> character_attributes;
	float auto_jump_min = 0.0f;
	float auto_jump_max = 0.0f;
	std::unordered_set<std::string> items;
	std::map<std::string, std::map<std::string, float>> item_attributes;
};

#endif