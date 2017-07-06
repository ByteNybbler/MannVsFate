#include "bot_generator.h"
#include "rand_util.h"
#include <iostream>
#include <algorithm>

bot_generator::bot_generator()
	: random_names("data/names/verbs.txt", "data/names/titles.txt", "data/names/adjectives.txt", "data/names/nouns.txt")
{}

void bot_generator::set_possible_classes(const std::vector<player_class>& classes)
{
	possible_classes = classes;
}

void bot_generator::set_pressure_decay_rate(float in)
{
	pressure_decay_rate = in;
}

void bot_generator::set_giant_chance(float in)
{
	giant_chance = in;
}

void bot_generator::set_boss_chance(float in)
{
	boss_chance = in;
}

void bot_generator::set_engies_enabled(bool in)
{
	engies_enabled = in;
}

void bot_generator::set_scale_mega(float in)
{
	scale_mega = in;
}

void bot_generator::set_generating_doombot(bool in)
{
	generating_doombot = in;
}

tfbot_meta bot_generator::generate_bot()
{
	// Let's generate a random TFBot.
	tfbot_meta bot_meta;
	tfbot& bot = bot_meta.get_bot();

	bot_meta.is_doom = generating_doombot;

	chance_mult = 1.0f;

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

	// Choose a weapon restriction.
	//if (bot.cl != player_class::engineer)
	//{
	// Restricting Spies' weapons may cause them to not be able to sap?
	if (!bot_meta.is_doom && bot.cl != player_class::spy)
	{
		if (rand_chance(0.3f))
		{
			std::vector<std::string> restrictions;

			// Spies do not have primary weapons.
			if (bot.cl != player_class::spy)
			{
				restrictions.emplace_back("PrimaryOnly");
			}
			restrictions.emplace_back("SecondaryOnly");
			restrictions.emplace_back("MeleeOnly");
			bot.weapon_restrictions = restrictions.at(rand_int(0, restrictions.size()));
		}
	}
	//}

	/*
	if (rand_chance(0.1f))
	{
	item_class = possible_classes.at(rand_int(0, possible_classes.size()));
	}
	*/

	// Set the bot's health based on its class.
	bot.health = get_class_default_health(bot.cl);

	// Get the bot's class icon.
	bot_meta.set_base_class_icon(get_class_icon(bot.cl));
	const std::string item_class_icon = get_class_icon(item_class);

	// Give the bot items here, prior to any chance of becoming a giant!
	list_reader item_reader;
	const std::string initial_path = "data/items/" + item_class_icon + '/';
	const std::string initial_path_bot = "data/items/" + bot.class_icon + '/';

	const std::string file_secondary = initial_path + "secondary.txt";
	const std::string file_melee = initial_path + "melee.txt";
	item_reader.load(file_secondary);
	item_reader.load(file_melee);
	const std::string secondary = item_reader.get_random(file_secondary);
	const std::string melee = item_reader.get_random(file_melee);
	bot.items.emplace_back(secondary);
	bot.items.emplace_back(melee);

	std::string primary;

	if (bot.cl == player_class::spy)
	{
		// Spies have some extra weapon slots.
		// pda2 = Cloaks
		// building = Sappers

		const std::string file_pda2 = initial_path_bot + "pda2.txt";
		item_reader.load(file_pda2);
		const std::string pda2 = item_reader.get_random(file_pda2);
		bot.items.emplace_back(pda2);

		// Changing a Spy's sapper makes them incapable of sapping for some reason.
		// At this point in time, sapper swaps are disabled.
		/*
		const std::string file_building = initial_path_bot + "building.txt";
		item_reader.load(file_building);
		const std::string building = item_reader.get_random(file_building);
		bot.items.emplace_back(building);
		*/
	}
	else if (item_class != player_class::spy)
	{
		// Spies do not have primary weapons.

		const std::string file_primary = initial_path + "primary.txt";
		item_reader.load(file_primary);
		primary = item_reader.get_random(file_primary);
		bot.items.emplace_back(primary);
	}
	else
	{
		const std::string file_primary = initial_path_bot + "primary.txt";
		item_reader.load(file_primary);
		primary = item_reader.get_random(file_primary);
		bot.items.emplace_back(primary);
	}

	bool is_buff_soldier = false;
	bool has_explosives = false;
	bool demo_can_charge = false;
	// Only override projectiles if there's no crash risk.
	bool projectile_override_crash_risk = false;

	// Choose a class icon based on the weapons chosen.
	switch (bot.cl)
	{
	case player_class::demoman:
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			if (melee == "The Eyelander" ||
				melee == "The Scotsman's Skullcutter" ||
				melee == "The Horseless Headless Horseman's Headtaker" ||
				melee == "The Claidheamohmor" ||
				melee == "The Persian Persuader" ||
				melee == "Festive Eyelander")
			{
				bot_meta.set_base_class_icon("demoknight");
			}
			else if (melee == "The Half-Zatoichi")
			{
				bot_meta.set_base_class_icon("demoknight_samurai");
			}
		}
		else if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
		{
			if (primary == "The Loose Cannon")
			{
				// Bots don't know how to use the loose cannon.
				bot_meta.pressure *= 0.7f;
			}
			if (primary != "The Bootlegger")
			{
				has_explosives = true;
			}
		}
		if (secondary == "The Chargin' Targe" ||
			secondary == "The Splendid Screen" ||
			secondary == "The Tide Turner")
		{
			demo_can_charge = true;
		}
		break;

	case player_class::heavyweapons:
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			if (melee == "The Killing Gloves of Boxing")
			{
				bot_meta.set_base_class_icon("heavy_champ");
			}
			else if (melee == "Gloves of Running Urgently" ||
				melee == "Festive Gloves of Running Urgently")
			{
				bot_meta.set_base_class_icon("heavy_gru");
			}
			else if (melee == "The Holiday Punch")
			{
				bot.attributes.emplace_back("AlwaysCrit");
				bot_meta.is_always_crit = true;
				bot_meta.set_base_class_icon("heavy_mittens");
			}
			else if (melee == "Fists of Steel")
			{
				bot_meta.set_base_class_icon("heavy_steelfist");
			}
		}
		else if (bot.weapon_restrictions == "SecondaryOnly")
		{
			if (secondary == "TF_WEAPON_SHOTGUN_PRIMARY" ||
				secondary == "The Family Business" ||
				secondary == "Festive Shotgun 2014" ||
				secondary == "Panic Attack Shotgun")
			{
				bot_meta.set_base_class_icon("heavy_shotgun");
			}
		}
		else if (bot.weapon_restrictions != "MeleeOnly")
		{
			if (primary == "The Huo Long Heatmaker")
			{
				bot_meta.set_base_class_icon("heavy_heater");
			}
		}
		break;

	case player_class::pyro:
		if (bot.weapon_restrictions == "SecondaryOnly")
		{
			if (secondary == "The Flare Gun" ||
				secondary == "The Scorch Shot" ||
				secondary == "Festive Flare Gun" ||
				secondary == "The Detonator" ||
				secondary == "The Manmelter")
			{
				bot_meta.set_base_class_icon("pyro_flare");
				if (secondary == "The Detonator")
				{
					has_explosives = true;
				}
			}
		}
		break;

	case player_class::scout:
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			if (melee == "TF_WEAPON_BAT" ||
				melee == "Festive Bat 2011")
			{
				bot_meta.set_base_class_icon("scout_bat");
			}
			else if (melee == "The Sandman")
			{
				bot_meta.set_base_class_icon("scout_stun");
			}
			else if (melee == "The Holy Mackerel")
			{
				bot_meta.set_base_class_icon("scout_fish");
			}
		}
		else
		{
			if (primary == "The Shortstop")
			{
				bot_meta.set_base_class_icon("scout_shortstop");
			}
		}
		if (secondary == "Bonk! Atomic Punch" || secondary == "Festive Bonk 2014")
		{
			bot_meta.set_base_class_icon("scout_bonk");
			bot.items.emplace_back("Bonk Helm");
			bot_meta.pressure *= 4.0f;
			// Make sure that we can never have giant Bonk Scouts...
			bot_meta.perma_small = true;
		}
		break;

	case player_class::sniper:
		if (secondary == "The Cozy Camper")
		{
			bot_meta.set_base_class_icon("sniper_camper");
		}
		if (bot.weapon_restrictions == "SecondaryOnly")
		{
			if (secondary == "Jarate" || secondary == "Festive Jarate")
			{
				bot_meta.set_base_class_icon("sniper_jarate");
			}
		}
		else
		{
			if (primary == "The Huntsman" ||
				primary == "Festive Huntsman")
			{
				bot_meta.set_base_class_icon("sniper_bow");
			}
			else if (primary == "The Sydney Sleeper")
			{
				bot_meta.set_base_class_icon("sniper_sydneysleeper");
			}
			else if (primary == "The Classic" || primary == "The Machina")
			{
				// Sniper bots don't know how to fire The Classic and The Machina.
				// If they're using that weapon, that effectively makes them wimps.
				bot_meta.pressure *= 0.7f;
			}
		}
		break;

	case player_class::soldier:
		if (bot.weapon_restrictions != "MeleeOnly")
		{
			if (bot.weapon_restrictions != "SecondaryOnly")
			{
				has_explosives = true;
			}
			if (primary == "The Black Box" ||
				primary == "Festive Black Box")
			{
				bot_meta.set_base_class_icon("soldier_blackbox");
			}
			else if (primary == "The Liberty Launcher")
			{
				bot_meta.set_base_class_icon("soldier_libertylauncher");
			}
		}
		if (secondary == "The Battalion's Backup")
		{
			bot_meta.set_base_class_icon("soldier_backup");
			is_buff_soldier = true;
		}
		else if (secondary == "The Buff Banner" ||
			secondary == "Festive Buff Banner")
		{
			bot_meta.set_base_class_icon("soldier_buff");
			is_buff_soldier = true;
		}
		else if (secondary == "The Concheror")
		{
			bot_meta.set_base_class_icon("soldier_conch");
			is_buff_soldier = true;
		}
		break;

	case player_class::medic:
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "SecondaryOnly")
		{
			/*
			if (rand_chance(0.2f * chance_mult))
			{
			bot.attributes.emplace_back("ProjectileShield");
			bot.pressure *= 3.0f;
			}
			*/
			if (secondary == "The Vaccinator MVM")
			{
				const int type = rand_int(0, 3);
				switch (type)
				{
				case 0:
					bot.attributes.emplace_back("VaccinatorBullets");
					bot_meta.set_base_class_icon("medic_vaccinator_bullet");
					break;
				case 1:
					bot.attributes.emplace_back("VaccinatorBlast");
					bot_meta.set_base_class_icon("medic_vaccinator_blast");
					break;
				case 2:
					bot.attributes.emplace_back("VaccinatorFire");
					bot_meta.set_base_class_icon("medic_vaccinator_fire");
					break;
				}
				//bot_meta.pressure *= 1.1f;
			}
		}
		break;

	case player_class::spy:
		if (melee == "Conniver's Kunai")
		{
			bot_meta.set_base_class_icon("spy_kunai");
		}
		break;
	}

	if (bot.cl == player_class::engineer || bot_meta.is_doom)
	{
		// Make it so that engineers and doombots cannot pick up the bomb.
		bot.character_attributes.emplace_back("cannot pick up intelligence", 1);
	}
	if (item_class == player_class::sniper)
	{
		if ((bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly") && !bot_meta.is_always_fire_weapon &&
			bot_meta.get_base_class_icon() != "sniper_bow"
			//&& primary != "The Classic"
			)
		{
			bot.attributes.emplace_back("AlwaysFireWeapon");
			bot_meta.is_always_fire_weapon = true;
		}
	}

	if (bot.cl == player_class::spy)
	{
		//chance_mult *= 3.0f;
		bot_meta.pressure *= 3.5f;
	}
	else if (bot.cl == player_class::engineer)
	{
		bot_meta.pressure *= 1.5f;
	}
	else if (bot.cl == player_class::medic)
	{
		bot_meta.pressure *= 1.1f;
	}

	// A bot has a chance to be a giant.
	if ((!bot_meta.is_giant && !bot_meta.perma_small && rand_chance(giant_chance)) || bot_meta.is_doom)
	{
		make_bot_into_giant(bot_meta);
	}
	// Have a chance to tweak the health value.
	if (rand_chance(0.5f) || (bot_meta.is_giant && rand_chance(0.8f)))
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
		float upper_bound = pressure_decay_rate * 0.007f; // 0.002f;
		bot.health = static_cast<int>(static_cast<float>(bot.health) * rand_float(0.2f * chance_mult, upper_bound));
	}

	if (!bot_meta.is_always_crit && rand_chance(0.05f * chance_mult))
	{
		bot.attributes.emplace_back("AlwaysCrit");
		bot_meta.is_always_crit = true;
		bot_meta.update_class_icon();

		if (bot.cl != player_class::engineer)
		{
			if (bot.weapon_restrictions == "MeleeOnly")
			{
				bot_meta.pressure *= 1.5f;
			}
			else
			{
				bot_meta.pressure *= 4.5f;
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

	if (rand_chance(0.1f * chance_mult) || bot_meta.is_doom)
	{
		bot.attributes.emplace_back("Aggressive");
	}
	
	if (rand_chance(0.5f * chance_mult))
	{
		if (item_class == player_class::medic)
		{
			if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "SecondaryOnly")
			{
				bot.attributes.emplace_back("SpawnWithFullCharge");
				bot_meta.pressure *= 2.5f;
			}
		}
		else if (item_class == player_class::soldier)
		{
			if (is_buff_soldier)
			{
				bot.attributes.emplace_back("SpawnWithFullCharge");
				bot_meta.pressure *= 1.2f;
			}
		}
	}
	if ((rand_chance(0.1f * chance_mult) || bot_meta.is_doom) && !bot_meta.is_always_fire_weapon)
	{
		if (rand_chance(0.4f) || is_buff_soldier)
		{
			bot.attributes.emplace_back("HoldFireUntilFullReload");
			//bot_meta.pressure *= 1.1f;
		}
		else
		{
			bot.attributes.emplace_back("AlwaysFireWeapon");
			bot_meta.pressure *= 1.5f;
			bot_meta.is_always_fire_weapon = true;
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

	if (item_class == player_class::demoman)
	{
		if (demo_can_charge && rand_chance(0.3f * chance_mult))
		{
			float charge_increase = rand_float(1.0f, 15.0f);

			bot_meta.pressure *= ((charge_increase - 1.0f) * 0.03f) + 1.0f;
			if (rand_chance(0.01f * chance_mult))
			{
				charge_increase *= 10000.0f;
				bot_meta.pressure *= 1.2f;
			}

			bot.character_attributes.emplace_back("charge time increased", charge_increase);
		}
	}
	else if (item_class == player_class::pyro)
	{
		if (bot.weapon_restrictions != "MeleeOnly" && bot.weapon_restrictions != "SecondaryOnly")
		{
			if (rand_chance(0.1f * chance_mult))
			{
				float r = rand_float(-10.0f, 10.0f);
				if (rand_chance(0.1f * chance_mult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("airblast pushback scale", r);
			}
			if (rand_chance(0.1f * chance_mult))
			{
				float r = rand_float(-10.0f, 10.0f);
				if (rand_chance(0.1f * chance_mult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("airblast vertical pushback scale", r);
			}
			if (rand_chance(0.1f * chance_mult))
			{
				float r = rand_float(0.1f, 10.0f);
				if (rand_chance(0.1f * chance_mult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("deflection size multiplier", r);
				//bot_meta.pressure *= 1.3f;
			}
		}
	}

	if (bot.cl == player_class::engineer)
	{
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.3f, 10.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy building health bonus", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.3f, 5.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry damage bonus", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.1f, 3.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry fire rate increased", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry radius increased", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engineer sentry build rate multiplier", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
		}
		if (rand_chance(0.05f * chance_mult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chance_mult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engineer teleporter build rate multiplier", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
		}
	}

	// Doombots will always have a high AutoJump so they can get around obstacles easier.
	if (rand_chance(0.08f * chance_mult) || bot_meta.is_doom)
	{
		bot.attributes.emplace_back("AutoJump");
		bot.auto_jump_min = rand_float(0.1f, 5.0f);
		if (rand_chance(0.5f))
		{
			bot.auto_jump_max = bot.auto_jump_min;
		}
		else
		{
			bot.auto_jump_max = bot.auto_jump_min + rand_float(0.1f, 5.0f);
		}
		bot_meta.pressure *= 1.2f;

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
			bot.character_attributes.emplace_back("increased jump height", increased_jump_height);
			bot.character_attributes.emplace_back("cancel falling damage", 1);
			if (increased_jump_height > 1.0f)
			{
				bot_meta.pressure *= 1.2f;

				if (rand_chance(0.5f))
				{
					bot.items.emplace_back("The B.A.S.E. Jumper");

					bot_meta.pressure *= ((increased_jump_height - 1.0f) * 0.7f) + 1.0f;
				}
			}
		}

		if (rand_chance(0.5f))
		{
			bot.character_attributes.emplace_back("bot custom jump particle", 1);
		}

		if (item_class == player_class::demoman)
		{
			if (demo_can_charge && rand_chance(0.2f * chance_mult))
			{
				bot.attributes.emplace_back("AirChargeOnly");
				bot_meta.pressure *= 0.9f;
			}
		}
	}
	if (
		//!is_giant &&
		rand_chance(0.15f))
	{
		// Giants are scale 1.75 by default.
		bot.scale = rand_float(0.6f, 1.75f);
	}

	if (rand_chance(0.1f * chance_mult))
	{
		bot.max_vision_range = 30000.0f;
		if (bot.weapon_restrictions != "MeleeOnly")
		{
			bot_meta.pressure *= 1.2f;
		}
	}
	bool instant_reload = false;
	if (rand_chance(0.1f * chance_mult))
	{
		float fire_rate_bonus = rand_float(0.1f, 2.0f);
		bot.character_attributes.emplace_back("fire rate bonus", fire_rate_bonus);
		bot.character_attributes.emplace_back("clip size bonus", 100000);
		if (fire_rate_bonus >= 1.0f)
		{
			bot_meta.pressure /= ((fire_rate_bonus - 1.0f) * 0.3f) + 1.0f;
		}
		else
		{
			bot_meta.pressure /= ((fire_rate_bonus - 1.0f) * 0.6f) + 1.0f;;

			switch (bot.cl)
			{
			case player_class::soldier:
				if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
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
			bot.character_attributes.emplace_back("faster reload rate", change);
			bot_meta.pressure /= ((change - 1.0f) * 0.3f) + 1.0f;

			if (change < 1.0f)
			{
				switch (bot.cl)
				{
				case player_class::soldier:
					if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
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
			bot.character_attributes.emplace_back("faster reload rate", -1);
			instant_reload = true;
			if ((bot.cl == player_class::heavyweapons || bot.cl == player_class::sniper) &&
				(bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly"))
			{
				// No reloading means Heavy's minigun shoots bullets at an insane rate.
				// Same for Sniper's rifles.
				bot_meta.pressure *= 5.0f;
				if (bot.cl == player_class::sniper)
				{
					//bot_meta.pressure *= 5.0f;
					constexpr float damage_factor = 0.1f;
					bot_meta.damage_bonus *= damage_factor;
					bot_meta.pressure /= damage_factor;
					projectile_override_crash_risk = true;
				}
			}
			else
			{
				if (bot.weapon_restrictions != "MeleeOnly")
				{
					bot_meta.pressure *= 1.1f;
				}
			}

			switch (bot.cl)
			{
			case player_class::soldier:
				if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
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
	if (rand_chance(0.1f * chance_mult) && bot.weapon_restrictions != "MeleeOnly")
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
		bot.character_attributes.emplace_back("bullets per shot bonus", rad);
		bot_meta.pressure *= ((rad - 1.0f) * 0.8f) + 1.0f;
	}
	if (rand_chance(0.1f * chance_mult) && secondary != "Bonk! Atomic Punch" && secondary != "Festive Bonk 2014")
	{
		const float change = rand_float(0.01f, 2.0f);
		bot.character_attributes.emplace_back("effect bar recharge rate increased", change);
		if (change < 1.0f)
		{
			bot_meta.pressure /= ((change - 1.0f) * 0.1f) + 1.0f;
		}
	}
	//if (bot.weapon_restrictions != "MeleeOnly")
	//{
	if (rand_chance(0.1f * chance_mult) && bot.weapon_restrictions != "MeleeOnly")
	{
		const float change = rand_float(0.01f, 10.0f);
		bot.character_attributes.emplace_back("projectile speed increased", change);
		bot_meta.pressure *= ((change - 1.0f) * 0.2f) + 1.0f;
	}
	if (rand_chance(0.1f * chance_mult))
	{
		int change = 3;
		while (rand_chance(0.5f) && change < 360)
		{
			change *= 3;
		}
		bot.character_attributes.emplace_back("projectile spread angle penalty", change);
	}
	if (rand_chance(0.05f * chance_mult))
	{
		int change = 3;
		bot_meta.pressure *= 1.05f;
		while (rand_chance(0.5f) && change < 360)
		{
			change *= 3;
			bot_meta.pressure *= 1.05f;
		}
		bot.character_attributes.emplace_back("weapon spread bonus", change);
	}
	//}
	if (rand_chance(0.03f * chance_mult))
	{
		if (rand_chance(0.5f))
		{
			bot.character_attributes.emplace_back("damage causes airblast", 1);
		}
		else
		{
			float look_velocity = rand_float(-10.0f, 10.0f);
			bot.character_attributes.emplace_back("apply look velocity on damage", look_velocity);
		}
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			bot_meta.pressure *= 1.1f;
		}
		else
		{
			bot_meta.pressure *= 2.0f;
		}
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
		bot.character_attributes.emplace_back("head scale", head_size);
	}

	if (item_class == player_class::demoman)
	{
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
		{
			if (primary == "The Loose Cannon")
			{
				projectile_override_crash_risk = true;
			}
		}
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "SecondaryOnly")
		{
			projectile_override_crash_risk = true;
		}
	}
	if (item_class == player_class::heavyweapons)
	{
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
		{
			//if (instant_reload)
			//{
				projectile_override_crash_risk = true;
			//}
		}
	}
	if (!projectile_override_crash_risk)
	{
		if (rand_chance(0.2f * chance_mult))
		{
			int proj_type = rand_int(1, 27);

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
				has_explosives = true;
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

			bot.character_attributes.emplace_back("override projectile type", proj_type);

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
		bot.character_attributes.emplace_back("bleeding duration", 5.0f);
		// Knife particles.
		bot.character_attributes.emplace_back("attach particle effect static", 43);
		bot_meta.pressure *= 2.0f;
	}
	if (rand_chance(0.04f * chance_mult))
	{
		bot.character_attributes.emplace_back("Set DamageType Ignite", 1);
		// Add some cool fire particles.
		bot.character_attributes.emplace_back("attach particle effect static", 13);
		if (item_class != player_class::pyro && bot.cl != player_class::engineer)
		{
			bot_meta.pressure *= 2.0f;
		}
		if (rand_chance(0.1f * chance_mult))
		{
			// Burn pretty much forever.
			bot.character_attributes.emplace_back("weapon burn time increased", 1000.0f);
			bot_meta.pressure *= 1.3f;
		}
		if (rand_chance(0.5f))
		{
			const float r = rand_float(0.01f, 4.0f);
			bot.character_attributes.emplace_back("weapon burn dmg increased", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
			/*
			if (r > 1.0f)
			{
			bot.pressure *= r;
			}
			else
			{
			bot.pressure *= 0.7f;
			}
			*/
		}
	}
	if (rand_chance(0.05f * chance_mult))
	{
		if (rand_chance(0.5f))
		{
			float slow_chance = rand_float(0.1f, 1.0f);
			bot.character_attributes.emplace_back("slow enemy on hit", slow_chance);
			bot_meta.pressure *= 1.0f + slow_chance;
		}
		else
		{
			bot.character_attributes.emplace_back("slow enemy on hit major", 1);
			bot_meta.pressure *= 1.2f;
		}
	}
	if (rand_chance(0.1f * chance_mult) && has_explosives)
	{
		bot.character_attributes.emplace_back("no self blast dmg", 1);
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
	if (rand_chance(0.02f))
	{
		bot.character_attributes.emplace_back("use large smoke explosion", 1);
		bot_meta.pressure *= 1.2f;
	}
	else
	*/
	if (rand_chance(0.05f * chance_mult))
	{
		if (has_explosives)
		{
			float rad = rand_float(0.1f, 5.0f);
			bot.character_attributes.emplace_back("Blast radius increased", rad);
			bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
			if (rad > 1.0f)
			{
				bot.character_attributes.emplace_back("use large smoke explosion", 1);
			}
		}
	}
	if (item_class == player_class::soldier || item_class == player_class::sniper || item_class == player_class::pyro)
	{
		if (rand_chance(0.1f))
		{
			const float rad = rand_float(0.1f, 20.0f);
			bot.character_attributes.emplace_back("increase buff duration", rad);
			//bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
		}
	}
	if (item_class == player_class::spy)
	{
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
			bot.character_attributes.emplace_back("sapper damage bonus", change);
		}
		if (rand_chance(0.2f))
		{
			float change = rand_float(0.01f, 2.0f);
			if (rand_chance(0.1f))
			{
				change *= 5.0f;
			}
			bot.character_attributes.emplace_back("sapper health bonus", change);
		}
		if (rand_chance(0.5f))
		{
			bot.character_attributes.emplace_back("sapper degenerates buildings", 1);
		}
	}
	if (item_class == player_class::heavyweapons)
	{
		// If the Heavy is inclined to use its minigun...
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly")
		{
			if (rand_chance(0.1f))
			{
				bot.character_attributes.emplace_back("attack projectiles", 1);
				bot_meta.set_base_class_icon("heavy_deflector");
				bot_meta.pressure *= 1.5f;
			}
		}
	}
	if (bot.weapon_restrictions == "MeleeOnly")
	{
		if (!bot_meta.is_always_fire_weapon && rand_chance(0.1f))
		{
			bot.character_attributes.emplace_back("hit self on miss", 1);
			bot_meta.pressure *= 0.9f;
		}
	}
	if (rand_chance(0.02f * chance_mult))
	{
		float r = rand_float(-10000.0f, 10000.0f);
		bot.character_attributes.emplace_back("apply z velocity on damage", r);
		// Cauldron Bubbles particle effect.
		bot.character_attributes.emplace_back("attach particle effect static", 39);
		bot_meta.pressure *= 1.5f;
		if (r < -100.0f)
		{
			bot_meta.pressure *= 1.5f;
		}
	}
	/*
	if (rand_chance(0.05f * chance_mult))
	{
	bot.character_attributes.emplace_back("projectile penetration", 1);
	bot.pressure *= 1.1f;
	}
	*/
	if (rand_chance(0.01f * chance_mult))
	{
		bot.character_attributes.emplace_back("attach particle effect static", rand_int(1, 48));
		bot.health *= 2;
	}

	// Set a random skill level for the bot.
	std::vector<std::string> skills({ "Easy", "Normal", "Hard", "Expert" });
	int skill_index = rand_int(0, skills.size());
	bot.skill = skills.at(skill_index);
	float skill_pressure = (skill_index * 0.5f) + 1.0f;
	bot_meta.pressure *= skill_pressure;

	if (bot_meta.damage_bonus >= 1.0f)
	{
		bot_meta.pressure *= bot_meta.damage_bonus;
		//bot_meta.pressure *= bot_meta.damage_bonus * 2.0f;
	}
	else
	{
		bot_meta.pressure *= ((bot_meta.damage_bonus - 1.0f) * 0.2f) + 1.0f;
	}

	bot.character_attributes.emplace_back("damage bonus", bot_meta.damage_bonus);

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
	bot_meta.pressure *= bot_meta.move_speed_bonus;
	if (bot_meta.move_speed_bonus > 1.0f)
	{
		// Fast robots will get an even higher threat level.
		bot_meta.pressure *= bot_meta.move_speed_bonus;
	}

	bot.character_attributes.emplace_back("move speed bonus", bot_meta.move_speed_bonus);

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

	return bot_meta;
}

void bot_generator::make_bot_into_giant_pure(tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();

	bot_meta.make_giant();
	bot_meta.is_giant = true;
	bot.scale = -1.0f;
	// Add some giant-related attributes.
	bot.character_attributes.emplace_back("airblast vulnerability multiplier", rand_float(0.3f, 0.7f));
	bot.character_attributes.emplace_back("damage force reduction", rand_float(0.3f, 0.7f));

	// Since giants can't be knocked around as easily, let's up the pressure a bit.
	bot_meta.pressure *= 1.5f;

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

	// A giant has a chance to be a BOSS!!!
	if (rand_chance(boss_chance) || bot_meta.is_doom)
	{
		bot_meta.is_boss = true;
		bot.health *= 5;
		bot_meta.move_speed_bonus *= 0.5;
		bot.attributes.emplace_back("UseBossHealthBar");
		chance_mult *= 4.0f;

		bot.scale = scale_mega;

		bot.character_attributes.emplace_back("attach particle effect static", rand_int(1, 48));

		// Now that you're a boss, choose a special class icon.
		switch (bot.cl)
		{
		case player_class::demoman:
			if (bot.weapon_restrictions == "PrimaryOnly")
			{
				bot_meta.set_base_class_icon("demo_bomber");
				bot.items.emplace_back("Prince Tavish's Crown");
			}
			break;

		case player_class::heavyweapons:
			if (bot.weapon_restrictions == "PrimaryOnly")
			{
				bot_meta.set_base_class_icon("heavy_chief");
				bot.items.emplace_back("War Head");
			}
			else if (bot_meta.get_base_class_icon() == "heavy_gru")
			{
				bot_meta.set_base_class_icon("heavy_urgent");
			}
			break;

		case player_class::scout:
			/*
			if (bot_meta.get_base_class_icon() == "scout_stun")
			{
				bot.class_icon = "scout_stun_giant_armored";
			}
			*/
			break;

		case player_class::soldier:
			if (bot.weapon_restrictions == "MeleeOnly")
			{
				bot_meta.set_base_class_icon("soldier_major_crits");
				bot.items.emplace_back("Full Metal Drill Hat");
			}
			else if (bot.weapon_restrictions == "SecondaryOnly")
			{
				bot_meta.set_base_class_icon("soldier_sergeant_crits");
				bot.items.emplace_back("Tyrant's Helm");
			}
			break;
		}
	}
}