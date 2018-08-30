#ifndef ITEM_CATEGORY_H
#define ITEM_CATEGORY_H

#include "player_class.h"
#include <vector>

// A data structure that describes a category of TF2 item.
struct item_category
{
	// The name of the item category.
	// This doubles as the name of the files containing the list of items in this category.
	// The files will all have the same name, but may be located in different folders,
	// depending on which player classes the items belong to.
	std::string category_name;
	// A collection of all of the player classes that have unique items in this category.
	std::vector<player_class> specific_classes;
	// Whether this item category is usable by all classes.
	bool is_for_all_classes = false;
};

#endif