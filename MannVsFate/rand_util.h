// Utility functions for assistance in generating random numbers.

#ifndef RAND_UTIL_H
#define RAND_UTIL_H

#include <vector>

// Returns a random integer between bottom (inclusive) and top (exclusive).
int rand_int(const int bottom, const int top);
// Returns a random integer between 0 (inclusive) and top (exclusive).
int rand_int(const int top);

// Returns a random float between bottom (inclusive) and top (exclusive).
float rand_float(const float bottom, const float top);

// Randomly returns true (chance*100)% of the time.
bool rand_chance(const float chance);

template <class T>
T rand_element_from_many_collections(const std::vector<std::vector<T>*>& collections)
{
	unsigned int total_size = 0;
	for (unsigned int i = 0; i < collections.size(); ++i)
	{
		total_size += collections.at(i)->size();
	}
	unsigned int chosen_index = rand_int(total_size);
	for (unsigned int i = 0; i < collections.size(); ++i)
	{
		size_t current_collection_size = collections.at(i)->size();
		if (chosen_index < current_collection_size)
		{
			return collections.at(i)->at(chosen_index);
		}
		else
		{
			chosen_index -= current_collection_size;
		}
	}
	throw std::exception("rand_util::rand_element_from_many_collections: Exceeded collection bounds.");
}

// Gets a random element from an enum. Works only if all of the following conditions are satisfied:
// 1. The first element of the enum is 0.
// 2. All of the elements of the enum are contiguous.
// 3. The last element of the enum is named "count".
/* Currently commented out due to unreliability between translation units.
template <typename T>
T rand_enum()
{
	int c = rand_int(0, static_cast<int>(T::count));
	return static_cast<T>(c);
}
*/

#endif