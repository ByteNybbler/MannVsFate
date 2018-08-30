#include "json_reader_item_category.h"
#include <fstream>

json_reader_item_category::json_reader_item_category()
{
	std::ifstream categories_file("data/cosmetics.json");
	if (!categories_file)
	{
		const std::string exstr = "json_reader_item_category exception: Couldn't find categories file \"data/cosmetics.json\".";
		throw std::exception(exstr.c_str());
	}

	// Read the file data into the json.
	try
	{
		categories_file >> categories_json;
	}
	catch (const std::exception&)
	{
		throw "cosmetics.json exception: JSON parse error.";
	}

	categories_file.close();
}

std::vector<item_category> json_reader_item_category::read()
{
	std::vector<item_category> result;

	// TODO: Iterate through each node of cosmetics.json.

	return result;
}