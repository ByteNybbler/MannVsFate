#ifndef LIST_READER_H
#define LIST_READER_H

#include <map>
#include <vector>

class list_reader
{
public:
	// Load a list from a file. If the file is already loaded, this function does nothing.
	void load(const std::string& source_filename);

	// Get a random element of a certain list.
	std::string get_random(const std::string& source) const;

private:
	std::map<std::string, std::vector<std::string>> lists;
};

#endif