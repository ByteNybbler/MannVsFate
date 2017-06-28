#include "random_name_generator.h"
#include "rand_util.h"

// If 1, debug info of get_random_name will be printed to std::cout.
#define RANDOM_NAME_DEBUG 0

#if RANDOM_NAME_DEBUG == 1

#include <iostream>
#include <string>

#endif

random_name_generator::random_name_generator(const std::string& verbs, const std::string& titles,
	const std::string& adjectives, const std::string& nouns)
	: verbs(verbs), titles(titles), adjectives(adjectives), nouns(nouns)
{
	lr.load(verbs);
	lr.load(titles);
	lr.load(adjectives);
	lr.load(nouns);
}

std::string random_name_generator::get_random_name() const
{
	string ss = "";
	while (ss == "") {
		std::string verb, title, adjective, noun1, noun2;
		if (rand_int(0, 2) == 0)
		{
			verb = lr.get_random(verbs) + ' ';
			title = "the ";
		}
		if (rand_int(0, 2) == 0)
		{
			title = lr.get_random(titles) + ' ';
		}
		if (rand_int(0, 2) == 0)
		{
			adjective = lr.get_random(adjectives) + ' ';
		}
		if (rand_int(0, 2) == 0)
		{
			noun1 = lr.get_random(nouns) + ' ';
		}
		// Always have at least a noun no matter what.
		noun2 = lr.get_random(nouns);
		ss = verb + title + adjective + noun1 + noun2;
		if (ss.length() >= 32) {
			ss = "";
		}
	}

#if RANDOM_NAME_DEBUG == 1
	if (verb != "")
	{
		std::cout << "Verb: " << verb << std::endl;
	}
	if (title != "")
	{
		std::cout << "Title: " << title << std::endl;
	}
	if (adjective != "")
	{
		std::cout << "Adjective: " << adjective << std::endl;
	}
	if (noun1 != "")
	{
		std::cout << "Noun 1: " << noun1 << std::endl;
	}
	if (noun2 != "")
	{
		std::cout << "Noun 2: " << noun2 << std::endl;
	}
	std::cout << std::endl;
#endif

	return verb + title + adjective + noun1 + noun2;
}
