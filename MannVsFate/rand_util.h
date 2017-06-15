// Utility functions for assistance in generating random numbers.

#ifndef RAND_UTIL_H
#define RAND_UTIL_H

// Returns a random integer between bottom (inclusive) and top (exclusive).
int rand_int(const int bottom, const int top);

// Returns a random float between bottom (inclusive) and top (exclusive).
float rand_float(const float bottom, const float top);

// Randomly returns true (chance*100)% of the time.
bool rand_chance(const float chance);

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