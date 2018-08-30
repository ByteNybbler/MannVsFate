#include "cosmetics_generator.h"
#include "tfbot.h"
#include "rand_util.h"
#include "player_class.h"

const std::string cosmetics_generator::root_path = "data/items/";

cosmetics_generator::cosmetics_generator(std::vector<item_category> categories)
	: categories(categories),
	cosmetic_chance(0.8f)
{}

void cosmetics_generator::add_cosmetic(std::string file_name, const std::vector<player_class> non_root_classes, bool has_root_file)
{
	if (rand_chance(cosmetic_chance))
	{
		std::vector<std::string> sources;
		if (has_root_file)
		{
			// Use the root file as a source.
			sources.emplace_back(root_path + file_name + ".txt");
		}
		// Whether the bot's class matches any of the non-root classes.
		bool bot_matches = false;
		for (unsigned int i = 0; i < non_root_classes.size(); ++i)
		{
			if (non_root_classes.at(i) == bot->cl)
			{
				bot_matches = true;
				break;
			}
		}
		if (bot_matches)
		{
			// Use the class-specific file as a source.
			sources.emplace_back(root_path + get_class_icon(bot->cl) + '/' + file_name + ".txt");
		}
		if (!sources.empty())
		{
			const std::string item_name = item_reader.get_random(sources);
			bot->items.emplace(item_name);
		}
	}
}

void cosmetics_generator::add_cosmetics(tfbot* bot)
{
	this->bot = bot;

	for (unsigned int i = 0; i < categories.size(); ++i)
	{
		item_category& cat = categories.at(i);
		add_cosmetic(cat.category_name, cat.specific_classes, cat.is_for_all_classes);
	}

	/*
	add_cosmetic("arm_tattoos", { player_class::scout, player_class::heavyweapons, player_class::engineer, player_class::sniper });
	add_cosmetic("arms", { player_class::demoman, player_class::engineer, player_class::heavyweapons, player_class::medic,
		player_class::pyro, player_class::scout, player_class::sniper, player_class::soldier });
	add_cosmetic("back", { player_class::demoman, player_class::engineer, player_class::medic, player_class::pyro, player_class::scout });
	add_cosmetic("beard", { player_class::demoman, player_class::engineer, player_class::heavyweapons, player_class::medic,
		 player_class::sniper, player_class::soldier, player_class::spy }, true);
	add_cosmetic("belt", { player_class::demoman, player_class::engineer });
	add_cosmetic("cigar", { player_class::medic, player_class::soldier, });
	add_cosmetic("ears", {}, true);
	add_cosmetic("face", { player_class::medic, player_class::pyro, player_class::soldier, player_class::spy });
	add_cosmetic("feet", { player_class::demoman, player_class::engineer, player_class::heavyweapons, player_class::medic,
		player_class::scout, player_class::sniper, player_class::soldier, player_class::spy }, true);
	add_cosmetic("flair", {}, true);
	add_cosmetic("floater", {}, true);
	add_cosmetic("glasses", { player_class::demoman, player_class::engineer, player_class::heavyweapons, player_class::medic,
		player_class::pyro, player_class::scout, player_class::sniper, player_class::soldier, player_class::spy }, true);
	add_cosmetic("grenades", )
	*/
}