#include "bot_generator.h"
#include "rand_util.h"
#include "pressure_manager.h"
#include "cosmetics_generator.h"
#include <algorithm>

// Set to 0 to disable debug messages for the bot generator.
#define BOT_GENERATOR_DEBUG 0

#if BOT_GENERATOR_DEBUG
#include <iostream>
#endif

bot_generator::bot_generator(const pressure_manager& pm, cosmetics_generator& cosgen)
	: wave_pressure(pm),
	cosmetics(cosgen),
	random_names("data/names/verbs.txt", "data/names/titles.txt", "data/names/adjectives.txt", "data/names/nouns.txt"),
	giant_chance(0.1f),
	boss_chance(0.15f),
	giant_chance_increase(0.05f),
	boss_chance_increase(0.01f),
	engies_enabled(true),
	generating_doombot(false),
	possible_classes{ player_class::scout,
	player_class::soldier,
	player_class::pyro,
	player_class::demoman,
	player_class::heavyweapons,
	player_class::engineer,
	player_class::medic,
	player_class::sniper,
	player_class::spy },
	give_bots_cosmetics(false),
	fire_chance(0.025f),
	bleed_chance(0.2f),
	nonbosses_can_get_bleed(false),
	minimum_bot_scale(0.6f),
	maximum_bot_scale(1.75f),
	minimum_giant_scale(1.25f),
	bot_scale_chance(0.15f),
	force_bot_scale(-1.0f)
{}

void bot_generator::set_possible_classes(const std::vector<player_class>& classes)
{
	possible_classes = classes;
}

void bot_generator::set_giant_chance(float in)
{
	giant_chance = in;
}

void bot_generator::set_boss_chance(float in)
{
	boss_chance = in;
}

void bot_generator::set_giant_chance_increase(float in)
{
	giant_chance_increase = in;
}

void bot_generator::set_boss_chance_increase(float in)
{
	boss_chance_increase = in;
}

void bot_generator::set_engies_enabled(bool in)
{
	engies_enabled = in;
}

void bot_generator::set_scale_mega(float in)
{
	scale_mega = in;
}

float bot_generator::get_scale_mega()
{
	return scale_mega;
}

void bot_generator::set_scale_doom(float in)
{
	scale_doom = in;
}

float bot_generator::get_scale_doom()
{
	return scale_doom;
}

void bot_generator::set_generating_doombot(bool in)
{
	generating_doombot = in;
}

void bot_generator::set_give_bots_cosmetics(bool in)
{
	give_bots_cosmetics = in;
}

void bot_generator::set_fire_chance(float in)
{
	fire_chance = in;
}

void bot_generator::set_bleed_chance(float in)
{
	bleed_chance = in;
}

void bot_generator::set_nonbosses_can_get_bleed(bool in)
{
	nonbosses_can_get_bleed = in;
}

void bot_generator::set_bot_scale_chance(float in)
{
	bot_scale_chance = in;
}

void bot_generator::set_minimum_bot_scale(float in)
{
	minimum_bot_scale = in;
}

void bot_generator::set_maximum_bot_scale(float in)
{
	maximum_bot_scale = in;
}

void bot_generator::set_minimum_giant_scale(float in)
{
	minimum_giant_scale = in;
}

void bot_generator::set_force_bot_scale(float in)
{
	force_bot_scale = in;
}

tfbot_meta bot_generator::generate_bot()
{
#if BOT_GENERATOR_DEBUG
	std::cout << "Started generating new TFBot." << std::endl;
#endif

	// Let's generate a random TFBot.
	tfbot_meta bot_meta;
	tfbot& bot = bot_meta.get_bot();

	bot_meta.is_doom = generating_doombot;

	chance_mult = 1.0f;

	const float decay_rate_per_player = wave_pressure.get_pressure_decay_rate_per_player();

	// If engineers are disabled, remove them from the possible classes vector.
	if (!engies_enabled)
	{
		possible_classes.erase(std::remove(possible_classes.begin(), possible_classes.end(), player_class::engineer), possible_classes.end());
	}

	// Make sure that doombots cannot be spies.
	bool has_removed_spy = false;
	if (bot_meta.is_doom)
	{
		if (std::find(possible_classes.begin(), possible_classes.end(), player_class::spy) != possible_classes.end())
		{
			possible_classes.erase(std::remove(possible_classes.begin(), possible_classes.end(), player_class::spy), possible_classes.end());
			has_removed_spy = true;
		}
	}

	// Give the bot a random name!
	bot.name = random_names.get_random_name();
	// Randomize the bot's class.
	//std::cout << "Number of possible classes: " << possible_classes.size() << std::endl;
	bot.cl = possible_classes.at(rand_int(0, possible_classes.size()));
	player_class item_class = bot.cl;

	if (has_removed_spy)
	{
		possible_classes.emplace_back(player_class::spy);
	}

#if BOT_GENERATOR_DEBUG
	std::cout << "Chose bot class." << std::endl;
#endif

	// Set the bot's health based on its class.
	bot.health = get_class_default_health(bot.cl);

	// Decide whether the bot will be a giant and/or a boss.
	if (rand_chance(giant_chance) || bot_meta.is_doom)
	{
		bot_meta.shall_be_giant = true;
		// A giant has a chance to be a BOSS!!!
		if (rand_chance(boss_chance) || bot_meta.is_doom)
		{
			bot_meta.shall_be_boss = true;
			chance_mult *= 4.0f;
			//std::cout << "Potential boss robot confirmed." << std::endl;
		}
	}

	// Get the bot's class icon.
	bot_meta.set_base_class_icon(get_class_icon(bot.cl));
	const std::string item_class_icon = get_class_icon(item_class);

	// Give the bot items!
	const std::string initial_path = "data/items/" + item_class_icon + '/';
	//const std::string initial_path_bot = "data/items/" + bot.class_icon + '/';

	const std::string file_secondary = initial_path + "secondary.txt";
	const std::string file_melee = initial_path + "melee.txt";
	//item_reader.load(file_secondary);
	//item_reader.load(file_melee);
	const std::string secondary_name = item_reader.get_random(file_secondary);
	const std::string melee_name = item_reader.get_random(file_melee);
	//bot.items.emplace_back(secondary);
	//bot.items.emplace_back(melee);
	weapon& secondary = bot_meta.add_weapon(secondary_name, weapon_reader);
	weapon& melee = bot_meta.add_weapon(melee_name, weapon_reader);

#if BOT_GENERATOR_DEBUG
	std::cout << "Loaded and added secondary and melee weapons." << std::endl;
	std::cout << "Number of names owned by secondary: " << secondary.names.size() << std::endl;
	std::cout << "First name of secondary: " << secondary.first_name() << std::endl;
#endif

#if BOT_GENERATOR_DEBUG
	std::cout << "Finished randomizing secondary and melee weapons." << std::endl;
#endif

	if (item_class == player_class::spy)
	{
		// Spies have some extra weapon slots.
		// pda2 = Cloaks
		// building = Sappers

		const std::string file_pda2 = initial_path + "pda2.txt";
		//item_reader.load(file_pda2);
		const std::string pda2_name = item_reader.get_random(file_pda2);
		//bot.items.emplace_back(pda2);
		weapon& pda2 = bot_meta.add_weapon(pda2_name, weapon_reader);

		// Changing a Spy's sapper makes them incapable of sapping for some reason.
		// At this point in time, sapper swaps are disabled.
		/*
		const std::string file_building = initial_path + "building.txt";
		item_reader.load(file_building);
		const std::string building = item_reader.get_random(file_building);
		bot.items.emplace_back(building);
		*/

		if (melee.is_a("Conniver's Kunai"))
		{
			bot_meta.set_base_class_icon("spy_kunai");
		}
	}
	else
	{
		// Spies do not have primary weapons.

		const std::string file_primary = initial_path + "primary.txt";
		//item_reader.load(file_primary);
		const std::string primary_name = item_reader.get_random(file_primary);
		//bot.items.emplace_back(primary);
		weapon& primary = bot_meta.add_weapon(primary_name, weapon_reader);

		randomize_weapon(primary, bot_meta);

		// Choose a weapon restriction.
		// Restricting Spies' weapons may cause them to not be able to sap buildings.
		if (!bot_meta.is_doom)
		{
#if BOT_GENERATOR_DEBUG
			std::cout << "Choosing weapon restriction." << std::endl;
#endif

			if (rand_chance(0.3f))
			{
				std::vector<weapon_restrictions> restrictions;

				// Spies do not have primary weapons.
				if (bot.cl != player_class::spy)
				{
					restrictions.emplace_back(weapon_restrictions::primary);
				}
				restrictions.emplace_back(weapon_restrictions::secondary);
				restrictions.emplace_back(weapon_restrictions::melee);
				bot.weapon_restriction = restrictions.at(rand_int(0, restrictions.size()));

				if (bot.weapon_restriction == weapon_restrictions::secondary)
				{
					if (!secondary.can_be_switched_to)
					{
						// Choose a weapon restriction that's NOT SecondaryOnly.
						restrictions.erase(std::remove(restrictions.begin(), restrictions.end(),
							weapon_restrictions::secondary), restrictions.end());
						bot.weapon_restriction = restrictions.at(rand_int(0, restrictions.size()));
					}
				}
				if (bot.weapon_restriction == weapon_restrictions::primary)
				{
					if (!primary.can_be_switched_to)
					{
						// Choose a weapon restriction that's NOT SecondaryOnly.
						restrictions.erase(std::remove(restrictions.begin(), restrictions.end(),
							weapon_restrictions::primary), restrictions.end());
						bot.weapon_restriction = restrictions.at(rand_int(0, restrictions.size()));
					}
					// Check secondary again in case primary was hit first.
					if (bot.weapon_restriction == weapon_restrictions::secondary)
					{
						if (!secondary.can_be_switched_to)
						{
							// Choose a weapon restriction that's NOT SecondaryOnly.
							restrictions.erase(std::remove(restrictions.begin(), restrictions.end(),
								weapon_restrictions::secondary), restrictions.end());
							bot.weapon_restriction = restrictions.at(rand_int(0, restrictions.size()));
						}
					}
				}
			}

#if BOT_GENERATOR_DEBUG
			std::cout << "Chose weapon restriction." << std::endl;
#endif
		}

		randomize_weapon(secondary, bot_meta);
		randomize_weapon(melee, bot_meta);

		if (bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::primary)
		{
			if (primary.is_a("The Huo Long Heatmaker"))
			{
				bot_meta.set_base_class_icon("heavy_heater");
			}
			else if (primary.is_a("The Shortstop"))
			{
				bot_meta.set_base_class_icon("scout_shortstop");
			}
			else if (primary.is_a("The Huntsman"))
			{
				bot_meta.set_base_class_icon("sniper_bow");
			}
			else if (primary.is_a("The Sydney Sleeper"))
			{
				bot_meta.set_base_class_icon("sniper_sydneysleeper");
			}
			else if (primary.is_a("The Black Box"))
			{
				bot_meta.set_base_class_icon("soldier_blackbox");
			}
			else if (primary.is_a("The Liberty Launcher"))
			{
				bot_meta.set_base_class_icon("soldier_libertylauncher");
			}

			if (primary.bots_too_dumb_to_use)
			{
				bot_meta.pressure *= 0.7f;
			}
			if (primary.is_a("tf_weapon_sniperrifle"))
			{
				bot.attributes.emplace("AlwaysFireWeapon");
				bot_meta.is_always_fire_weapon = true;
				// Snipers are able to hit players from basically anywhere with their rifles.
				// This fact combined with their obvious aimbot makes them very deadly.
				if (!primary.bots_too_dumb_to_use)
				{
					bot_meta.pressure *= 1.5f;
				}
			}
			if (primary.is_a("tf_weapon_flamethrower"))
			{
				// Phlogistinators cannot airblast.
				if (!primary.is_a("The Phlogistinator"))
				{
					// Airblast Pyros are scary shit.
					bot_meta.pressure *= 1.5f;
					if (rand_chance(0.1f * chance_mult))
					{
						float r = rand_float(-10.0f, 10.0f);
						if (rand_chance(0.1f * chance_mult))
						{
							r *= 100.0f;
						}
						bot.character_attributes["airblast pushback scale"] = r;
					}
					if (rand_chance(0.1f * chance_mult))
					{
						float r = rand_float(-10.0f, 10.0f);
						if (rand_chance(0.1f * chance_mult))
						{
							r *= 100.0f;
						}
						bot.character_attributes["airblast vertical pushback scale"] = r;
					}
					if (rand_chance(0.1f * chance_mult))
					{
						float r = rand_float(0.1f, 10.0f);
						if (rand_chance(0.1f * chance_mult))
						{
							r *= 100.0f;
						}
						bot.character_attributes["deflection size multiplier"] = r;
						//bot_meta.pressure *= 1.3f;
					}
				}
			}
			if (primary.is_a("tf_weapon_minigun"))
			{
				bot_meta.pressure *= 1.3f;
				if (rand_chance(0.1f))
				{
					bot.character_attributes["attack projectiles"] = 1;
					bot_meta.set_base_class_icon("heavy_deflector");
					bot_meta.pressure *= 1.5f;
				}
			}
		}
	}

	if (bot.weapon_restriction == weapon_restrictions::melee)
	{
		bot_meta.pressure *= 0.7f;

		if (melee.is_a("tf_weapon_sword"))
		{
			bot_meta.set_base_class_icon("demoknight");
		}
		else if (melee.is_a("tf_weapon_katana"))
		{
			bot_meta.set_base_class_icon("demoknight_samurai");
		}
		else if (melee.is_a("The Killing Gloves of Boxing"))
		{
			bot_meta.set_base_class_icon("heavy_champ");
		}
		else if (melee.is_a("Gloves of Running Urgently"))
		{
			bot_meta.set_base_class_icon("heavy_gru");
		}
		else if (melee.is_a("The Holiday Punch"))
		{
			bot.attributes.emplace("AlwaysCrit");
			bot_meta.is_always_crit = true;
			bot_meta.set_base_class_icon("heavy_mittens");
		}
		else if (melee.is_a("Fists of Steel"))
		{
			bot_meta.set_base_class_icon("heavy_steelfist");
		}
		else if (melee.is_a("The Sandman"))
		{
			bot_meta.set_base_class_icon("scout_stun");
		}
		else if (melee.is_a("The Holy Mackerel"))
		{
			bot_meta.set_base_class_icon("scout_fish");
		}
		else if (melee.is_a("tf_weapon_bat"))
		{
			bot_meta.set_base_class_icon("scout_bat");
		}

		if (!bot_meta.is_always_fire_weapon && rand_chance(0.1f))
		{
			bot.character_attributes["hit self on miss"] = 1;
			bot_meta.pressure *= 0.85f;
		}
	}
	else if (bot.weapon_restriction == weapon_restrictions::secondary)
	{
		if (secondary.is_a("tf_weapon_shotgun"))
		{
			bot_meta.set_base_class_icon("heavy_shotgun");
		}
		else if (secondary.is_a("tf_weapon_flaregun"))
		{
			bot_meta.set_base_class_icon("pyro_flare");
		}
		else if (secondary.is_a("Jarate"))
		{
			bot_meta.set_base_class_icon("sniper_jarate");
		}
	}

	if (secondary.is_a("Bonk! Atomic Punch"))
	{
		bot_meta.set_base_class_icon("scout_bonk");
		if (!give_bots_cosmetics)
		{
			bot.items.emplace("Bonk Helm");
		}
		bot_meta.pressure *= 4.0f;
		// Make sure that we can never have giant Bonk Scouts...
		bot_meta.perma_small = true;
	}
	else if (secondary.is_a("The Cozy Camper"))
	{
		bot_meta.set_base_class_icon("sniper_camper");
	}
	else if (secondary.is_a("The Battalion's Backup"))
	{
		bot_meta.set_base_class_icon("soldier_backup");
	}
	else if (secondary.is_a("The Buff Banner"))
	{
		bot_meta.set_base_class_icon("soldier_buff");
	}
	else if (secondary.is_a("The Concheror"))
	{
		bot_meta.set_base_class_icon("soldier_conch");
	}
	else if (secondary.is_a("The Vaccinator MVM"))
	{
		const int type = rand_int(0, 3);
		switch (type)
		{
		case 0:
			bot.attributes.emplace("VaccinatorBullets");
			bot_meta.set_base_class_icon("medic_vaccinator_bullet");
			break;
		case 1:
			bot.attributes.emplace("VaccinatorBlast");
			bot_meta.set_base_class_icon("medic_vaccinator_blast");
			break;
		case 2:
			bot.attributes.emplace("VaccinatorFire");
			bot_meta.set_base_class_icon("medic_vaccinator_fire");
			break;
		}
		//bot_meta.pressure *= 1.1f;
	}

#if BOT_GENERATOR_DEBUG
	std::cout << "Done choosing first class icons." << std::endl;
#endif

	if (give_bots_cosmetics)
	{
		cosmetics.add_cosmetics(&bot);
	}

	// A bot has a chance to be a giant.
	if ((bot_meta.shall_be_giant && !bot_meta.is_giant && !bot_meta.perma_small) || bot_meta.is_doom)
	{
		make_bot_into_giant(bot_meta);
	}

	// tf_wearable_demoshield allows Demoman to charge.
	if (secondary.is_a("tf_wearable_demoshield"))
	{
		bot_meta.pressure *= 1.1f;

		if (rand_chance(0.3f * chance_mult))
		{
			float charge_increase = rand_float(1.0f, 15.0f);

			//bot_meta.pressure *= ((charge_increase - 1.0f) * 0.03f) + 1.0f;
			if (rand_chance(0.01f * chance_mult))
			{
				charge_increase *= 10000.0f;
				//bot_meta.pressure *= 1.2f;
			}

			bot.character_attributes["charge time increased"] = charge_increase;

			if (rand_chance(0.1f))
			{
				bot.attributes.emplace("AirChargeOnly");
				bot_meta.pressure *= 0.9f;
			}
		}
	}

	if (bot_meta.is_doom)
	{
		bot.character_attributes["cannot pick up intelligence"] = 1;
	}

	if (bot.cl == player_class::engineer)
	{
		//bot_meta.pressure *= 1.5f;
		bot_meta.pressure *= 0.5f;
		bot_meta.wait_between_spawns_multiplier *= 10.0f;

		chance_mult *= 2.0f;

		// Make it so that engineers cannot pick up the bomb.
		bot.character_attributes["cannot pick up intelligence"] = 1;

		if (bot_meta.is_boss)
		{
			constexpr float engie_buff_chance = 0.1f; // 0.05f
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.3f, 10.0f); // 0.3f, 10.0f
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engy building health bonus"] = r;
				//bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
			}
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.3f, 5.0f);
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engy sentry damage bonus"] = r;
				//bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
			}
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.1f, 3.0f);
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engy sentry fire rate increased"] = r;
				//bot_meta.pressure /= ((r - 1.0f) * 0.3f) + 1.0f;
			}
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.01f, 5.0f);
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engy sentry radius increased"] = r;
				//bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
			}
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.01f, 5.0f);
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engineer sentry build rate multiplier"] = r;
				//bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
			}
			if (rand_chance(engie_buff_chance * chance_mult))
			{
				float r = rand_float(0.01f, 5.0f);
				if (rand_chance(0.01f * chance_mult))
				{
					r *= 0.01f;
				}
				bot.character_attributes["engineer teleporter build rate multiplier"] = r;
				//bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
			}
		}
	}
	else if (bot.cl == player_class::spy)
	{
		bot_meta.pressure *= 3.5f;

		// Wacky sapper-specific stuff.
		if (rand_chance(0.2f))
		{
			float change = rand_float(0.1f, 1.0f);
			if (rand_chance(0.1f))
			{
				change *= rand_float(0.1f, 10.0f);
			}
			if (rand_chance(0.02f))
			{
				change *= -1.0f;
			}
			bot.character_attributes["sapper damage bonus"] = change;
		}
		if (rand_chance(0.2f))
		{
			float change = rand_float(0.01f, 2.0f);
			if (rand_chance(0.1f))
			{
				change *= 5.0f;
			}
			bot.character_attributes["sapper health bonus"] = change;
		}
		if (rand_chance(0.5f))
		{
			bot.character_attributes["sapper degenerates buildings"] = 1;
		}
	}
	else if (bot.cl == player_class::medic)
	{
		bot_meta.pressure *= 1.1f;
		bot_meta.wait_between_spawns_multiplier *= 1.5f;
	}

	if (rand_chance(0.1f * chance_mult) || bot_meta.is_doom)
	{
		bot.attributes.emplace("Aggressive");
	}

	if ((rand_chance(0.1f * chance_mult) || bot_meta.is_doom) && !bot_meta.is_always_fire_weapon)
	{
		if (rand_chance(0.4f))
		{
			bot.attributes.emplace("HoldFireUntilFullReload");
			//bot_meta.pressure *= 1.1f;
		}
		else
		{
			bot.attributes.emplace("AlwaysFireWeapon");
			bot_meta.pressure *= 1.3f;
			bot_meta.is_always_fire_weapon = true;
		}
	}

	// Have a chance to tweak the health value.
	if (rand_chance(0.5f) || (bot_meta.is_giant && rand_chance(0.95f)))
	{
		float lower_bound = 0.2f;
		if (bot_meta.is_giant)
		{
			lower_bound += 0.4f;
			if (bot_meta.is_boss)
			{
				lower_bound += 0.4f;
			}
		}
		float upper_bound = wave_pressure.get_pressure_decay_rate() * 0.007f; // 0.002f;
		if (bot.cl == player_class::engineer)
		{
			lower_bound *= 2.0f;
			upper_bound *= 2.0f;
		}
		bot.health = static_cast<int>(static_cast<float>(bot.health) * rand_float(lower_bound, upper_bound));
	}

	// Original always_crit_chance was a flat 0.05f.
	const float always_crit_chance = decay_rate_per_player * 0.002f; // 0.001f
	//std::cout << "always_crit_chance: " << always_crit_chance << std::endl;
	//std::getchar();

	if (!bot_meta.is_always_crit && rand_chance(always_crit_chance * chance_mult))
	{
		bot.attributes.emplace("AlwaysCrit");
		bot_meta.is_always_crit = true;
		bot_meta.update_class_icon();

		if (bot.cl != player_class::engineer)
		{
			if (bot.weapon_restriction == weapon_restrictions::melee)
			{
				bot_meta.pressure *= get_muted_damage_pressure(1.1f);
			}
			else
			{
				//std::cout << "wave_pressure.get_pressure_decay_rate_per_player(): " << decay_rate_per_player << std::endl;
				//const float change_factor = decay_rate_per_player * 0.024f;
				//const float change = ((4.5f - 1.0f) / change_factor) + 1.0f;
				//bot_meta.pressure *= change;
				bot_meta.pressure *= get_muted_damage_pressure(4.5f);
				//std::cout << "AlwaysCrit pressure change: " << change << std::endl;
				//std::getchar();
				//bot_meta.pressure_health *= 2.0f;
			}
		}
	}
	if (rand_chance(0.05f * chance_mult))
	{
		float change = rand_float(0.5f, 3.0f);
		bot_meta.move_speed_bonus *= change;

		switch (bot.cl)
		{
		case player_class::scout:
			if (change > 1.0f && bot_meta.get_base_class_icon() == "scout")
			{
				bot_meta.set_base_class_icon("scout_fast");
			}
			break;

		case player_class::spy:
			if (change < 1.0f)
			{
				bot_meta.set_base_class_icon("spy_slow");
			}
		}
	}

	/*
	// Using this attribute may cause the bot to idle in its spawn without moving if there's no bomb.
	if (rand_chance(0.02f))
	{
		bot.attributes.emplace_back("IgnoreFlag");
	}
	*/
	/*
	if (rand_chance(0.05f * chance_mult))
	{
	bot.attributes.emplace_back("BulletImmune");
	bot.pressure *= 2.0f;
	}
	if (rand_chance(0.05f * chance_mult))
	{
	bot.attributes.emplace_back("BlastImmune");
	bot.pressure *= 2.0f;
	}
	if (rand_chance(0.05f * chance_mult))
	{
	bot.attributes.emplace_back("FireImmune");
	bot.pressure *= 2.0f;
	}
	*/
	// Parachute does nothing.
	/*
	if (rand_chance(0.1f * chance_mult))
	{
	bot.attributes.emplace_back("Parachute");
	}
	*/

	// Doombots will always have a high AutoJump so they can get around obstacles easier.
	if (rand_chance(0.08f * chance_mult) || bot_meta.is_doom)
	{
		bot.attributes.emplace("AutoJump");
		bot.auto_jump_min = rand_float(0.1f, 5.0f); // (0.1f, 5.0f);
		if (rand_chance(0.5f))
		{
			bot.auto_jump_max = bot.auto_jump_min;
		}
		else
		{
			bot.auto_jump_max = bot.auto_jump_min + rand_float(0.1f, 5.0f);
		}
		bot_meta.pressure *= 1.2f;
		//bot_meta.pressure_health *= 1.2f;

		if (rand_chance(0.5f) || bot_meta.is_doom)
		{
			float increased_jump_height;
			if (bot_meta.is_doom)
			{
				increased_jump_height = 10.0f;
			}
			else
			{
				increased_jump_height = rand_float(0.1f, 15.0f);
			}
			bot.character_attributes["increased jump height"] = increased_jump_height;
			bot.character_attributes["cancel falling damage"] = 1;
			if (increased_jump_height > 1.0f)
			{
				bot_meta.pressure *= 1.2f;
				//bot_meta.pressure_health *= 1.2f;

				if (rand_chance(0.5f))
				{
					bot_meta.add_weapon("The B.A.S.E. Jumper", weapon_reader);

					// If necessary, make the bot jump more often. Otherwise, it will take FOREVER to get out of spawn.
					if (bot.auto_jump_min < 1.0f)
					{
						const float difference = 1.0f - bot.auto_jump_min;
						bot.auto_jump_min += difference;
						bot.auto_jump_max += difference;
					}

					const float change = ((increased_jump_height - 1.0f) * 0.1f) + 1.0f;
					bot_meta.pressure *= change;
					//bot_meta.pressure_health *= change;
				}
			}
		}

		if (rand_chance(0.5f))
		{
			bot.character_attributes["bot custom jump particle"] = 1;
		}
	}
	if (rand_chance(bot_scale_chance))
	{
		// Giants are scale 1.75 by default.
		bot.scale = rand_float(minimum_bot_scale, maximum_bot_scale);
	}

	if (rand_chance(0.1f * chance_mult))
	{
		bot.max_vision_range = 30000.0f;
		if (bot.weapon_restriction != weapon_restrictions::melee)
		{
			bot_meta.pressure *= 1.2f;
		}
	}
	bool instant_reload = false;
	if (rand_chance(0.1f * chance_mult) || (bot.cl == player_class::engineer && rand_chance(0.8f)))
	{
		float fire_rate_bonus = rand_float(0.1f, 2.0f);
		bot.character_attributes["fire rate bonus"] = fire_rate_bonus;
		bot.character_attributes["clip size bonus"] = 100000;
		if (fire_rate_bonus >= 1.0f)
		{
			bot_meta.pressure /= ((fire_rate_bonus - 1.0f) * 0.3f) + 1.0f;
		}
		else
		{
			bot_meta.pressure /= ((fire_rate_bonus - 1.0f) * 0.6f) + 1.0f;

			switch (bot.cl)
			{
			case player_class::soldier:
				if (bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::primary)
				{
					bot_meta.set_base_class_icon("soldier_spammer");
				}
				break;

			case player_class::sniper:
				if (bot_meta.get_base_class_icon() == "sniper_bow")
				{
					bot_meta.set_base_class_icon("sniper_bow_multi");
				}
				break;
			}
		}
	}
	if (rand_chance(0.1f * chance_mult))
	{
		if (rand_chance(0.4f))
		{
			float change = rand_float(0.1f, 2.0f);
			if (rand_chance(0.01f))
			{
				change *= 3.0f;
			}
			bot.character_attributes["faster reload rate"] = change;
			bot_meta.pressure /= ((change - 1.0f) * 0.3f) + 1.0f;

			if (change < 1.0f)
			{
				switch (bot.cl)
				{
				case player_class::soldier:
					if (bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::primary)
					{
						bot_meta.set_base_class_icon("soldier_barrage");
					}
					break;

				case player_class::sniper:
					if (bot_meta.get_base_class_icon() == "sniper_bow")
					{
						bot_meta.set_base_class_icon("sniper_bow_multi");
					}
					break;
				}
			}
		}
		else
		{
			bool will_get_instant_reload = true;
			bool will_be_overlord_heavy_or_sniper = (bot.cl == player_class::heavyweapons || bot.cl == player_class::sniper) &&
				(bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::primary);

			if (will_be_overlord_heavy_or_sniper)
			{
				will_get_instant_reload = bot_meta.is_boss;
			}
			if (will_get_instant_reload)
			{
				bot.character_attributes["faster reload rate"] = -1;
				instant_reload = true;

				if (will_be_overlord_heavy_or_sniper)
				{
					// No reloading means Heavy's minigun shoots bullets at an insane rate.
					// Same for Sniper's rifles.
					bot_meta.pressure *= get_muted_damage_pressure(5.0f);
					if (bot.cl == player_class::sniper)
					{
						constexpr float damage_factor = 0.1f;
						bot_meta.damage_bonus *= damage_factor;
						bot_meta.pressure /= damage_factor;

						// Having a projectile override on such a rapid-fire weapon can crash the game on some systems.
						bot_meta.projectile_override_crash_risk = true;
					}

					// Nemesis Burst particle effect.
					bot.character_attributes["attach particle effect static"] = 3;
				}
				else
				{
					if (bot.weapon_restriction != weapon_restrictions::melee)
					{
						//bot_meta.pressure *= get_muted_damage_pressure(1.1f);
					}
				}

				switch (bot.cl)
				{
				case player_class::soldier:
					if (bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::primary)
					{
						bot_meta.set_base_class_icon("soldier_spammer");
					}
					break;

				case player_class::sniper:
					if (bot_meta.get_base_class_icon() == "sniper_bow")
					{
						bot_meta.set_base_class_icon("sniper_bow_multi");
					}
					break;
				}
			}
		}
	}
	if (rand_chance(0.1f * chance_mult))
	{
		float damage_bonus_mod;
		if (bot_meta.is_giant)
		{
			damage_bonus_mod = rand_float(0.1f, 3.0f);
		}
		else
		{
			damage_bonus_mod = rand_float(0.1f, 1.0f);
		}
		bot_meta.damage_bonus *= damage_bonus_mod;
	}
	if (secondary.is_a("tf_weapon_buff_item") && rand_chance(0.1f * chance_mult))
	{
		const float rad = rand_float(0.1f, 20.0f);
		bot.character_attributes["increase buff duration"] = rad;
	}
	if (rand_chance(0.05f * chance_mult))
	{
		float head_size = 0.001f;
		while (rand_chance(0.95f) && head_size < 10.0f)
		{
			head_size += 0.1f;
		}
		if (rand_chance(0.2f))
		{
			head_size *= -1.0f;
		}
		if (rand_chance(0.01f))
		{
			head_size *= 10.0f;
		}
		bot.character_attributes["head scale"] = head_size;
	}
	// The TeleportToHint attribute requires the other bots to have progressed a certain distance before this bot spawns.
	// If this wavespawn is not marked as support, this can lead to potentially unwinnable waves due to this bot never spawning in some conditions.
	/*
	if (bot.cl == player_class::engineer && rand_chance(0.2f))
	{
	bot.attributes.emplace_back("TeleportToHint");
	bot.pressure *= 2.0f;
	}
	*/
	if (rand_chance(0.05f * chance_mult))
	{
		if (rand_chance(0.5f))
		{
			float slow_chance = rand_float(0.1f, 1.0f);
			bot.character_attributes["slow enemy on hit"] = slow_chance;
			bot_meta.pressure *= get_muted_damage_pressure(1.0f + slow_chance);
		}
		else
		{
			bot.character_attributes["slow enemy on hit major"] = 1;
			bot_meta.pressure *= get_muted_damage_pressure(1.2f);
		}
	}
	/*
	if (rand_chance(0.01f))
	{
		bot.character_attributes.emplace_back("melee bounds multiplier", 360.0f);
		bot.character_attributes.emplace_back("melee range multiplier", 30000.0f);
		bot.character_attributes.emplace_back("Blast radius increased", 30000.0f);
		bot_meta.damage_bonus *= 0.01f;
		// Compensate for the massively decreased damage bonus.
		bot_meta.pressure *= 50.0f;
	}
	*/
	/*
	if (rand_chance(0.05f * chance_mult))
	{
	bot.character_attributes.emplace_back("projectile penetration", 1);
	bot.pressure *= 1.1f;
	}
	*/
	if (rand_chance(0.01f * chance_mult))
	{
		bot.character_attributes["attach particle effect static"] = rand_int(1, 48);
		bot.health *= 2;
	}

	// Set a random skill level for the bot.
	std::vector<std::string> skills({ "Easy", "Normal", "Hard", "Expert" });
	int skill_index = rand_int(0, skills.size());
	bot.skill = skills.at(skill_index);
	float skill_pressure = (skill_index * 0.5f) + 1.0f; // skill_index * 0.5f

	// Skilled Pyros are very, VERY good at airblasting, making them quite threatening to most classes.
	if (bot.cl != player_class::pyro)
	{
		skill_pressure = get_muted_damage_pressure(skill_pressure);
	}
	if (bot.cl != player_class::engineer)
	{
		bot_meta.pressure *= skill_pressure;
	}

	if (bot_meta.damage_bonus >= 1.0f)
	{
		bot_meta.pressure *= get_muted_damage_pressure(bot_meta.damage_bonus);
		//bot_meta.pressure *= bot_meta.damage_bonus * 2.0f;
	}
	else
	{
		bot_meta.pressure *= ((bot_meta.damage_bonus - 1.0f) * 0.2f) + 1.0f;
	}

	bot.character_attributes["damage bonus"] = bot_meta.damage_bonus;

	if (bot_meta.is_doom)
	{
		bot_meta.move_speed_bonus = 2.0f;
	}
	else
	{
		const float min_speed = 0.15f;
		if (bot_meta.move_speed_bonus < min_speed)
		{
			bot_meta.move_speed_bonus = min_speed;
		}
	}

	// Finalize how the bot's pressure is affected by its movement speed.
	if (bot.cl != player_class::engineer)
	{
		// Formerly just bot_meta.move_speed_bonus.
		float move_speed = bot_meta.calculate_absolute_move_speed();
		if (move_speed > 1.0f)
		{
			// Fast robots will get a higher threat level.
			bot_meta.pressure *= move_speed * move_speed;
		}
		else
		{
			// Slower robots' threat levels won't be affected as much.
			bot_meta.pressure *= ((move_speed - 1.0f) * 0.5f) + 1.0f;
		}
	}

	bot.character_attributes["move speed bonus"] = bot_meta.move_speed_bonus;

	check_bot_scale(bot_meta);
	if (bot.scale < 0.0f)
	{
		// Negative scale implies that the bot is normal-sized.
		// In the case of standard non-giant robots, do nothing.

		// Giants are tough enough. Let's not turn down their pressure.
		/*
		if (bot_meta.is_giant)
		{
			// Giants have a scale of 1.75.
			bot_meta.pressure /= ((1.75f - 1.0f) * 0.3f) + 1.0f;
		}
		*/
	}
	else if (bot.scale < 1.0f)
	{
		bot_meta.pressure /= bot.scale;
	}
	/*
	else
	{
		bot_meta.pressure /= ((bot.scale - 1.0f) * 0.3f) + 1.0f;
	}
	*/

	if (bot.health <= 0)
	{
		throw std::exception("bot_generator::generate_bot exception: Non-positive bot health!");
	}

	if (bot_meta.pressure < 0.0f)
	{
		throw std::exception("bot_generator::generate_bot exception: Negative bot pressure!");
	}

	// Set doombots to their proper scale.
	if (bot_meta.is_doom)
	{
		bot.scale = scale_doom;
	}

	return bot_meta;
}

void bot_generator::check_bot_scale(tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();

	if (bot_meta.is_giant && bot.scale < minimum_giant_scale && bot.scale > 0.0f)
	{
		bot.scale = minimum_giant_scale;
	}

	if (force_bot_scale >= 0.0f)
	{
		bot.scale = force_bot_scale;
	}
}

void bot_generator::make_bot_into_giant_pure(tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();

	bot_meta.make_giant();
	bot_meta.is_giant = true;
	bot.scale = -1.0f;
	// Add some giant-related attributes.
	bot.character_attributes["airblast vulnerability multiplier"] = rand_float(0.3f, 0.7f);
	bot.character_attributes["damage force reduction"] = rand_float(0.3f, 0.7f);

	// Since giants can't be knocked around as easily, let's up the pressure a bit.
	bot_meta.pressure *= 1.2f; // 1.5f;

	// If the class isn't Scout, incur a move speed penalty...
	if (bot.cl != player_class::scout)
	{
		if (bot.cl == player_class::spy)
		{
			bot_meta.move_speed_bonus *= 0.75f;
		}
		else
		{
			bot_meta.move_speed_bonus *= 0.5f;
		}
	}

	// Update the bot class icon.
	bot_meta.update_class_icon();
}

void bot_generator::make_bot_into_giant(tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();

	// Being a giant multiplies the TFBot's health by 15.
	bot.health *= 15;

	make_bot_into_giant_pure(bot_meta);
	check_bot_scale(bot_meta);

	if (bot_meta.shall_be_boss)
	{
		bot_meta.is_boss = true;

		bot.health *= 10; // 5;

		if (rand_chance(0.5f))
		{
			// Make the bot move a bit slower so the players have more time to deal with it!
			bot_meta.move_speed_bonus *= 0.5;
		}

		bot.attributes.emplace("UseBossHealthBar");

		bot.scale = scale_mega;

		bot.character_attributes["attach particle effect static"] = rand_int(1, 48);

		/*
		// Now that you're a boss, choose a special class icon.
		switch (bot.cl)
		{
		case player_class::demoman:
			if (bot.weapon_restriction == weapon_restrictions::primary)
			{
				bot_meta.set_base_class_icon("demo_bomber");
				bot.items.emplace("Prince Tavish's Crown");
			}
			break;

		case player_class::heavyweapons:
			if (bot.weapon_restriction == weapon_restrictions::primary)
			{
				bot_meta.set_base_class_icon("heavy_chief");
				bot.items.emplace("War Head");
			}
			else if (bot_meta.get_base_class_icon() == "heavy_gru")
			{
				bot_meta.set_base_class_icon("heavy_urgent");
			}
			break;

		case player_class::soldier:
			if (bot.weapon_restriction == weapon_restrictions::melee)
			{
				bot_meta.set_base_class_icon("soldier_major_crits");
				bot.items.emplace("Full Metal Drill Hat");
			}
			else if (bot.weapon_restriction == weapon_restrictions::secondary)
			{
				bot_meta.set_base_class_icon("soldier_sergeant_crits");
				bot.items.emplace("Tyrant's Helm");
			}
			break;
		}
		*/
	}
}

void bot_generator::wave_ended()
{
	giant_chance += giant_chance_increase;
	boss_chance += boss_chance_increase;
}

void bot_generator::randomize_weapon(weapon& wep, tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();

#if BOT_GENERATOR_DEBUG
	std::cout << "Inside randomize_weapon now. get_bot called successfully." << std::endl;
	std::cout << "Number of names owned by the weapon: " << wep.names.size() << std::endl;
#endif

	// Get a reference to the bot's item attributes
	std::map<std::string, float>& item_attributes = bot.item_attributes[wep.first_name()];

#if BOT_GENERATOR_DEBUG
	std::cout << "item_attributes references successfully." << std::endl;
#endif

	const bool is_main_weapon = wep.matches_restriction(bot.weapon_restriction);

	if (rand_chance(0.01f * chance_mult))
	{
		item_attributes["attach particle effect static"] = rand_int(1, 48);
		bot.health *= 2;
	}

	if (!wep.can_be_switched_to)
	{
		return;
	}

	if (!wep.does_damage)
	{
		if (is_main_weapon)
		{
			bot_meta.pressure *= 0.7f;
		}
		return;
	}

	if (wep.can_be_charged && !bot_meta.is_always_fire_weapon && rand_chance(0.5f * chance_mult))
	{
		bot.attributes.emplace("SpawnWithFullCharge");

		if (bot.cl == player_class::medic)
		{
			if (bot.weapon_restriction == weapon_restrictions::none || bot.weapon_restriction == weapon_restrictions::secondary)
			{
				bot_meta.pressure *= get_muted_damage_pressure(2.5f);
			}
		}
		else if (bot.cl == player_class::soldier)
		{
			bot_meta.pressure *= get_muted_damage_pressure(1.2f);
		}
	}

	if (wep.has_projectiles)
	{
		// Projectile overrides crash the game on some weapons.
		if (wep.projectile_override_crash_risk)
		{
			bot_meta.projectile_override_crash_risk = true;
		}
		if (rand_chance(0.1f * chance_mult))
		{
			int change = 3;
			while (rand_chance(0.5f) && change < 360)
			{
				change *= 3;
			}
			item_attributes["projectile spread angle penalty"] = change;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			int change = 3;
			if (is_main_weapon)
			{
				bot_meta.pressure *= get_muted_damage_pressure(1.05f);
			}
			while (rand_chance(0.5f) && change < 360)
			{
				change *= 3;
				if (is_main_weapon)
				{
					bot_meta.pressure *= get_muted_damage_pressure(1.05f);
				}
			}
			item_attributes["weapon spread bonus"] = change;
		}
		if (rand_chance(0.1f * chance_mult))
		{
			float rad;
			if (bot_meta.is_giant)
			{
				rad = rand_float(0.1f, 3.0f);
			}
			else
			{
				rad = rand_float(0.1f, 1.0f);
			}
			item_attributes["bullets per shot bonus"] = rad;
			if (is_main_weapon)
			{
				bot_meta.pressure *= ((rad - 1.0f) * 0.8f) + 1.0f;
			}
		}
		if (rand_chance(0.1f * chance_mult))
		{
			const float change = rand_float(0.01f, 10.0f);
			item_attributes["projectile speed increased"] = change;
			if (is_main_weapon)
			{
				bot_meta.pressure *= ((change - 1.0f) * 0.2f) + 1.0f;
			}
		}
		if (!bot_meta.projectile_override_crash_risk)
		{
			if (rand_chance(0.2f * chance_mult))
			{
				//int proj_type; // = rand_int(1, 27);

				std::vector<int> possibilities{
					1, // Bullet
					2, // Rocket
					5, // Syringe
					6, // Flare
				};

				if (wep.arc_fire)
				{
					// Some projectiles require arcs to fire properly.

					possibilities.emplace_back(3); // Pipebomb
					possibilities.emplace_back(8); // Huntsman Arrow
					possibilities.emplace_back(11); // Crusader's Crossbow Bolt
					possibilities.emplace_back(12); // Cow Mangler 5000 Projectile
					possibilities.emplace_back(13); // Righteous Bison Projectile
					possibilities.emplace_back(18); // Rescue Ranger Arrow
					possibilities.emplace_back(19); // Festive Huntsman Arrow
					possibilities.emplace_back(22); // Festive Jarate
					possibilities.emplace_back(23); // Festive Crusader's Crossbow Bolt
					possibilities.emplace_back(24); // Self Aware Beauty Mark
					possibilities.emplace_back(25); // Mutated Milk
				}

				//possibilities.emplace_back(26); // Grappling Hook

				int proj_type = possibilities.at(rand_int(0, possibilities.size()));;
				item_attributes["override projectile type"] = proj_type;

				/*
				// Fix invalid values.
				switch (proj_type)
				{

				case 2: // Rocket
				case 9: // Invalid
				case 10: // Invalid
				case 15: // Invalid
				case 16: // Invalid
				case 20: // Invalid
				case 21: // Invalid
					proj_type = 2; // Rocket
					break;

				case 4: // Stickybomb (Stickybomb Launcher)
				case 6: // Flare
				case 7: // Invalid
				case 14: // Stickybomb (Sticky Jumper)
					proj_type = 6; // Flare
					break;

				case 26: // Grappling hook
					break;
				}
				*/

				item_attributes["override projectile type"] = proj_type;

				/*
				if (proj_type == 2 && item_class != player_class::soldier)
				{
				bot_meta.pressure *= 1.5f;
				}
				if (proj_type == 6 && item_class != player_class::pyro)
				{
				bot_meta.pressure *= 1.5f;
				}
				*/
			}
		}
	}

	if (wep.has_effect_charge_bar)
	{
		if ((rand_chance(0.5f * chance_mult) || wep.is_a("tf_weapon_jar")))
		{
			float lower_range = 0.0001f;
			float upper_range = 2.0f;
			if (wep.is_a("Bonk! Atomic Punch"))
			{
				lower_range = 0.2f;
			}
			if (wep.is_a("tf_weapon_jar"))
			{
				upper_range = 0.12f;
			}
			const float change = rand_float(0.01f, upper_range);
			item_attributes["effect bar recharge rate increased"] = change;
			if (change < 1.0f)
			{
				if (is_main_weapon)
				{
					bot_meta.pressure /= ((change - 1.0f) * 0.05f) + 1.0f;
				}
			}
		}
	}

	if (bot_meta.is_boss || nonbosses_can_get_bleed)
	{
		if (!wep.burns && rand_chance(bleed_chance * chance_mult)) // 0.01f
		{
			// Enable bleeding.
			item_attributes["bleeding duration"] = 5.0f;
			wep.bleeds = true;

			// Knife particles.
			item_attributes["attach particle effect static"] = 43;

			if (is_main_weapon)
			{
				if (bot.weapon_restriction == weapon_restrictions::melee)
				{
					bot_meta.pressure *= 1.1f;
				}
				else
				{
					// Note that bleeding stacks if you're hit by multiple different bleed-inducing weapons!
					//bot_meta.pressure *= get_muted_damage_pressure(2.0f);
					bot_meta.pressure *= 1.5f;
					bot_meta.wait_between_spawns_multiplier *= 5.0f;
				}
			}
		}
	}

	if (!wep.bleeds && rand_chance(fire_chance * chance_mult))
	{
		// Enable burning.
		item_attributes["Set DamageType Ignite"] = 1;
		wep.burns = true;

		// Add some cool fire particles.
		item_attributes["attach particle effect static"] = 13;
		if (bot.cl != player_class::pyro && bot.cl != player_class::engineer)
		{
			if (is_main_weapon)
			{
				if (bot.weapon_restriction == weapon_restrictions::melee)
				{
					bot_meta.pressure *= get_muted_damage_pressure(1.1f);
				}
				else
				{
					bot_meta.pressure *= get_muted_damage_pressure(2.0f);
				}
			}
		}
		if (rand_chance(0.1f * chance_mult))
		{
			// Burn pretty much forever.
			item_attributes["weapon burn time increased"] = 1000.0f;
			if (is_main_weapon)
			{
				if (bot.weapon_restriction == weapon_restrictions::melee)
				{
					bot_meta.pressure *= get_muted_damage_pressure(1.05f);
				}
				else
				{
					bot_meta.pressure *= get_muted_damage_pressure(1.3f);
				}
			}
		}
		if (rand_chance(0.5f))
		{
			const float r = rand_float(0.01f, 4.0f);
			item_attributes["weapon burn dmg increased"] = r;
			if (is_main_weapon)
			{
				bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
			}
		}
	}

	if (wep.explodes)
	{
		if (rand_chance(0.1f * chance_mult))
		{
			bot.character_attributes["no self blast dmg"] = 1;
		}

		if (rand_chance(0.05f * chance_mult))
		{
			float rad = rand_float(0.1f, 5.0f);
			item_attributes["Blast radius increased"] = rad;
			if (rad > 4.0f)
			{
				item_attributes["use large smoke explosion"] = 1;
			}
			if (is_main_weapon)
			{
				bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
			}
		}
	}

	if (!wep.burns && !wep.bleeds)
	{
		if (rand_chance(0.03f * chance_mult))
		{
			if (rand_chance(0.5f))
			{
				item_attributes["damage causes airblast"] = 1;
			}
			else
			{
				float look_velocity = rand_float(-10.0f, 10.0f);
				item_attributes["apply look velocity on damage"] = look_velocity;
			}
			if (is_main_weapon)
			{
				if (bot.weapon_restriction == weapon_restrictions::melee)
				{
					bot_meta.pressure *= get_muted_damage_pressure(1.1f);
				}
				else
				{
					bot_meta.pressure *= get_muted_damage_pressure(2.0f);
				}
			}
		}
		if (rand_chance(0.02f * chance_mult))
		{
			float r = rand_float(-10000.0f, 10000.0f);
			item_attributes["apply z velocity on damage"] = r;
			// Cauldron Bubbles particle effect.
			item_attributes["attach particle effect static"] = 39;

			if (is_main_weapon)
			{
				bot_meta.pressure *= 1.5f;
				if (r < -100.0f)
				{
					bot_meta.pressure *= 1.5f;
				}
			}
		}
	}
}

float bot_generator::get_muted_damage_pressure(const float base) const
{
	const float change_factor = wave_pressure.get_pressure_decay_rate_per_player() * 0.024f;
	const float change = ((base - 1.0f) / change_factor) + 1.0f;
	/*
	std::cout << "bot_generator::get_muted_damage_pressure: change_factor / change / base:\n"
		<< change_factor << " / " << change << " / " << base << std::endl;
		*/
	// Make sure the muting doesn't actually make the number bigger.
	if (change > base)
	{
		return base;
	}
	else
	{
		return change;
	}
}