#ifndef COSMETICS_GENERATOR_H
#define COSMETICS_GENERATOR_H

#include "list_reader.h"
#include "item_category.h"

class tfbot;

// Generates cosmetics for a bot.
class cosmetics_generator
{
public:
	cosmetics_generator(std::vector<item_category> categories);
	// Add cosmetics (hats etc) to a robot.
	void add_cosmetics(tfbot* bot);

private:
	// Potentially adds a single cosmetic to a robot, depending on random chance.
	// file_name is the single name of the files that will be read for cosmetics.
	// The files will have the same name, but may be located in different folders.
	// non_root_classes are the classes beyond the root folder that are viable for this cosmetic.
	// has_root_file is whether or not a cosmetics file exists in the root directory for any class to draw from.
	void add_cosmetic(std::string file_name, const std::vector<player_class> non_root_classes, bool has_root_file = false);

	// A collection of the categories of cosmetics.
	std::vector<item_category> categories;
	// The bot that is receiving the cosmetics.
	tfbot* bot;
	// The item reader to use for each cosmetic file.
	list_reader item_reader;
	// The chance that any given cosmetic will be added.
	float cosmetic_chance;

	// The root path to use.
	static const std::string root_path;
};

#endif