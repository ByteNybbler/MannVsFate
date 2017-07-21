#ifndef TFBOT_H
#define TFBOT_H

#include "player_class.h"
#include "spawnable.h"
#include <string>
#include <vector>

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
	std::string weapon_restrictions = "";
	float max_vision_range = -1.0f;
	std::vector<std::string> attributes;
	std::vector<std::pair<std::string, float>> character_attributes;
	float auto_jump_min = 0.0f;
	float auto_jump_max = 0.0f;
	std::vector<std::string> items;
};

#endif