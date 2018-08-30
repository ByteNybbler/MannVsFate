#include "rand_util.h"
#include <random>

int rand_int(const int bottom, const int top)
{
	if (top <= bottom)
	{
		return top;
	}
	else
	{
		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<> dist(bottom, top);
		return static_cast<int>(std::floor(dist(e2)));
	}
}

int rand_int(const int top)
{
	return rand_int(0, top);
}

float rand_float(const float bottom, const float top)
{
	if (top <= bottom)
	{
		return top;
	}
	else
	{
		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<> dist(bottom, top);
		return static_cast<float>(dist(e2));
	}
}

bool rand_chance(const float chance)
{
	return (rand_float(0.0f, 1.0f) < chance);
}