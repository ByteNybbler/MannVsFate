#ifndef SPAWNABLE_H
#define SPAWNABLE_H

// Base class for spawnable enemies.
class spawnable
{
public:
	enum class type
	{
		tfbot,
		tank
	};

	virtual type get_type() const = 0;
};

#endif