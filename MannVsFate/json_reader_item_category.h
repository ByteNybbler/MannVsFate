#ifndef JSON_READER_ITEM_CATEGORY_H
#define JSON_READER_ITEM_CATEGORY_H

#include "item_category.h"
#include "json.hpp"
#include <vector>

class json_reader_item_category
{
public:
	json_reader_item_category();

	std::vector<item_category> read();

private:
	// Represents the root of the file.
	nlohmann::json categories_json;
};

#endif