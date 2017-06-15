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

std::string list_reader::get_random(const std::string& source) const
{
	const std::vector<std::string>& vec = lists.at(source);
	const int index = rand_int(0, vec.size());
	return vec.at(index);
}