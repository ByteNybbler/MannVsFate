#include "list_reader.h"
#include "rand_util.h"
#include <fstream>
#include <string>

// If 1, print debug messages to std::cout.
#define LIST_READER_DEBUG 0

#if LIST_READER_DEBUG == 1

#include <iostream>

#endif

void list_reader::load(const std::string& source_filename)
{
	// Check if there's already an entry for the file in the lists map.
	// If there already is an entry, this function does nothing, thus saving computation power.
	if (lists.find(source_filename) == lists.end())
	{
		std::ifstream file(source_filename);
		if (file)
		{
			// Instantiate a vector for the corresponding filename within the lists map.
			// This vector will contain the list of items read from the file.
			lists.emplace(source_filename, std::vector<std::string>());
			std::string phrase;
			while (std::getline(file, phrase))
			{
				lists.at(source_filename).emplace_back(phrase);

#if LIST_READER_DEBUG == 1
				std::cout << "Appended phrase " << phrase << " to vector " << source_filename << '.' << std::endl;
#endif
			}
			file.close();
		}
		else
		{
			const std::string exstr = "list_reader::load exception: File \"" + source_filename + "\" not found!";
			throw std::exception(exstr.c_str());
		}
	}
}

std::string list_reader::get_random(const std::string& source)
{
	load(source);
	const std::vector<std::string>& vec = lists.at(source);
	const int index = rand_int(0, vec.size());
	return vec.at(index);
}

std::string list_reader::get_random(const std::vector<std::string>& sources)
{
	std::vector<std::vector<std::string>*> vec;
	for (unsigned int i = 0; i < sources.size(); ++i)
	{
		load(sources[i]);
		vec.emplace_back(&lists.at(sources[i]));
	}
	return rand_element_from_many_collections(vec);
}