#ifndef RANDOM_NAME_GENERATOR_H
#define RANDOM_NAME_GENERATOR_H

#include "list_reader.h"

class random_name_generator
{
public:
	// Constructor. Defers to list reader for loading word lists by passing these filename arguments.
	random_name_generator(const std::string& verbs, const std::string& titles,
		const std::string& adjectives, const std::string& nouns);

	// Get a random name.
	std::string get_random_name() const;

private:
	list_reader lr;
	const std::string verbs, titles, adjectives, nouns;
};

#endif