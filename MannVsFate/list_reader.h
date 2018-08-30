#ifndef LIST_READER_H
#define LIST_READER_H

#include <map>
#include <vector>

class list_reader
{
public:
	// Get a random element from a certain list.
	std::string get_random(const std::string& source);
	// Get a random element from several lists.
	std::string get_random(const std::vector<std::string>& sources);

private:
	// Load a list from a file. If the file is already loaded, this function does nothing.
	void load(const std::string& source_filename);

	// The collection of lists loaded from files.
	std::map<std::string, std::vector<std::string>> lists;
};

#endif