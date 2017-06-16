#include "bot_generator.h"
#include "rand_util.h"
#include <iostream>

bot_generator::bot_generator()
	: random_names("names/verbs.txt", "names/titles.txt", "names/adjectives.txt", "names/nouns.txt")
{}

void bot_generator::set_possible_classes(const std::vector<player_class>& classes)
{
	possible_classes = classes;
}

void bot_generator::set_pressure_decay_rate(float in)
{
	pressure_decay_rate = in;
}

tfbot_meta bot_generator::generate_bot()
{
	// Let's generate a random TFBot.
	tfbot_meta bot_meta;
	tfbot& bot = bot_meta.get_bot();

	chanceMult = 1.0f;

	// Give the bot a random name!
	bot.name = random_names.get_random_name();
	// Randomize the bot's class.
	bot.cl = possible_classes.at(rand_int(0, possible_classes.size()));
	player_class item_class = bot.cl;

	// Choose a weapon restriction.
	//if (bot.cl != player_class::engineer)
	//{
	if (rand_chance(0.3f))
	{
		std::vector<std::string> restrictions;
		if (bot.cl != player_class::spy)
		{
			restrictions.emplace_back("PrimaryOnly");
		}
		restrictions.emplace_back("SecondaryOnly");
		restrictions.emplace_back("MeleeOnly");
		bot.weapon_restrictions = restrictions.at(rand_int(0, restrictions.size()));
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
	bot.class_icon = get_class_icon(bot.cl);
	const std::string item_class_icon = get_class_icon(item_class);

	// Give the bot items here, prior to any chance of becoming a giant!
	list_reader item_reader;
	const std::string initial_path = "items/" + item_class_icon + '/';
	const std::string initial_path_bot = "items/" + bot.class_icon + '/';

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

		const std::string file_pda2 = initial_path_bot + "pda2.txt";
		item_reader.load(file_pda2);
		const std::string pda2 = item_reader.get_random(file_pda2);
		bot.items.emplace_back(pda2);

		const std::string file_building = initial_path_bot + "building.txt";
		item_reader.load(file_building);
		const std::string building = item_reader.get_random(file_building);
		bot.items.emplace_back(building);
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

	if (!bot_meta.isAlwaysCrit && rand_chance(0.05f))
	{
		bot.attributes.emplace_back("AlwaysCrit");
		bot_meta.pressure *= 4.5f;
		bot_meta.isAlwaysCrit = true;
	}

	// If the bot is a Heavy, possibly set deflector properties.
	if (bot.cl == player_class::heavyweapons)
	{
		if (rand_chance(0.1f))
		{
			bot.character_attributes.emplace_back("attack projectiles", 1);
			/*
			if (is_deflector_giant)
			{
			make_bot_into_giant(bot, isGiant, move_speed_bonus, chanceMult, bot.pressure, isBoss, isAlwaysCrit);
			}
			else
			{
			permaSmall = true;
			}
			*/
			bot.class_icon = "heavy_deflector";
			bot_meta.pressure *= 2.5f;
		}
	}

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
				melee == "The Half-Zatoichi" ||
				melee == "The Persian Persuader" ||
				melee == "Festive Eyelander")
			{
				bot.class_icon = "demoknight";
			}
		}
		break;

	case player_class::heavyweapons:
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			if (melee == "The Killing Gloves of Boxing")
			{
				bot.class_icon = "heavy_champ";
			}
			else if (melee == "Gloves of Running Urgently" ||
				melee == "Festive Gloves of Running Urgently")
			{
				bot.class_icon = "heavy_gru";
			}
			else if (melee == "The Holiday Punch")
			{
				bot.class_icon = "heavy_mittens";
				bot.attributes.emplace_back("AlwaysCrit");
				bot_meta.isAlwaysCrit = true;
			}
			else if (melee == "Fists of Steel")
			{
				bot.class_icon = "heavy_steelfist";
			}
		}
		else if (bot.weapon_restrictions == "SecondaryOnly")
		{
			if (secondary == "TF_WEAPON_SHOTGUN_PRIMARY" ||
				secondary == "The Family Business" ||
				secondary == "Festive Shotgun 2014" ||
				secondary == "Panic Attack Shotgun")
			{
				bot.class_icon = "heavy_shotgun";
			}
		}
		else if (bot.weapon_restrictions != "MeleeOnly")
		{
			if (primary == "The Huo Long Heatmaker")
			{
				bot.class_icon = "heavy_heater";
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
				bot.class_icon = "pyro_flare";
			}
		}
		break;

	case player_class::scout:
		if (bot.weapon_restrictions == "MeleeOnly")
		{
			if (melee == "TF_WEAPON_BAT" ||
				melee == "Festive Bat 2011")
			{
				bot.class_icon = "scout_bat";
			}
			else if (melee == "The Sandman")
			{
				bot.class_icon = "scout_stun";
			}
		}
		else
		{
			if (primary == "The Shortstop")
			{
				bot.class_icon = "scout_shortstop";
			}
		}
		if (secondary == "Bonk! Atomic Punch" || secondary == "Festive Bonk 2014")
		{
			bot.class_icon = "scout_bonk";
			bot.items.emplace_back("Bonk Helm");
			bot_meta.pressure *= 2.5f;
		}
		break;

	case player_class::sniper:
		if (bot.weapon_restrictions == "SecondaryOnly")
		{
			bot.class_icon = "sniper_jarate";
		}
		else
		{
			if (primary == "The Huntsman" ||
				primary == "Festive Huntsman")
			{
				bot.class_icon = "sniper_bow";
			}
			else if (primary == "The Sydney Sleeper")
			{
				bot.class_icon = "sniper_sydneysleeper";
			}
		}
		break;

	case player_class::soldier:
		if (bot.weapon_restrictions != "MeleeOnly")
		{
			if (primary == "The Black Box" ||
				primary == "Festive Black Box")
			{
				bot.class_icon = "soldier_blackbox";
			}
			else if (primary == "The Liberty Launcher")
			{
				bot.class_icon = "soldier_libertylauncher";
			}
		}
		if (secondary == "The Battalion's Backup")
		{
			bot.class_icon = "soldier_backup";
		}
		else if (secondary == "The Buff Banner" ||
			secondary == "Festive Buff Banner")
		{
			bot.class_icon = "soldier_buff";
		}
		else if (secondary == "The Concheror")
		{
			bot.class_icon = "soldier_conch";
		}
		break;

	case player_class::medic:
		if (bot.weapon_restrictions == "" || bot.weapon_restrictions == "SecondaryOnly")
		{
			/*
			if (rand_chance(0.2f * chanceMult))
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
					bot.class_icon = "medic_vaccinator_bullet";
					break;
				case 1:
					bot.attributes.emplace_back("VaccinatorBlast");
					bot.class_icon = "medic_vaccinator_blast";
					break;
				case 2:
					bot.attributes.emplace_back("VaccinatorFire");
					bot.class_icon = "medic_vaccinator_fire";
					break;
				}
				//bot_meta.pressure *= 1.1f;
			}
		}
		break;
	}

	if (rand_chance(0.05f))
	{
		float change = rand_float(0.5f, 3.0f);
		bot_meta.move_speed_bonus *= change;
		bot_meta.pressure *= change;
	}

	if (bot.cl == player_class::engineer)
	{
		// Make it so that engineers cannot pick up the bomb.
		bot.character_attributes.emplace_back("cannot pick up intelligence", 1);
	}
	if (item_class == player_class::sniper)
	{
		if ((bot.weapon_restrictions == "" || bot.weapon_restrictions == "PrimaryOnly") && !bot_meta.isAlwaysFireWeapon &&
			bot.class_icon != "sniper_bow" && primary != "The Classic")
		{
			bot.attributes.emplace_back("AlwaysFireWeapon");
			bot_meta.isAlwaysFireWeapon = true;
		}
	}

	/*
	if (bot.cl == player_class::spy)
	{
		//chanceMult *= 3.0f;
		bot_meta.pressure *= 1.2f;
	}
	*/

	// A bot has a chance to be a giant.
	if (!bot_meta.isGiant && !bot_meta.permaSmall && rand_chance(0.1f))
	{
		make_bot_into_giant(bot_meta);
	}
	// Have a chance to tweak the health value.
	if ((bot_meta.isGiant && rand_chance(0.8f)) || rand_chance(0.5f))
	{
		float upper_bound = pressure_decay_rate * 0.007f; // 0.002f;
		bot.health = static_cast<int>(static_cast<float>(bot.health) * rand_float(0.2f * chanceMult, upper_bound));
	}
	if (rand_chance(0.1f * chanceMult))
	{
		bot.attributes.emplace_back("Aggressive");
	}
	if ((item_class == player_class::medic || item_class == player_class::soldier) && rand_chance(0.5f * chanceMult))
	{
		bot.attributes.emplace_back("SpawnWithFullCharge");
		bot_meta.pressure *= 2.5f;
	}
	if (rand_chance(0.1f * chanceMult) && !bot_meta.isAlwaysFireWeapon)
	{
		if (rand_chance(0.4f))
		{
			bot.attributes.emplace_back("HoldFireUntilFullReload");
			bot_meta.pressure *= 1.1f;
		}
		else
		{
			bot.attributes.emplace_back("AlwaysFireWeapon");
			bot_meta.pressure *= 1.5f;
			bot_meta.isAlwaysFireWeapon = true;
		}
	}
	if (rand_chance(0.02f))
	{
		bot.attributes.emplace_back("IgnoreFlag");
	}
	/*
	if (rand_chance(0.05f * chanceMult))
	{
	bot.attributes.emplace_back("BulletImmune");
	bot.pressure *= 2.0f;
	}
	if (rand_chance(0.05f * chanceMult))
	{
	bot.attributes.emplace_back("BlastImmune");
	bot.pressure *= 2.0f;
	}
	if (rand_chance(0.05f * chanceMult))
	{
	bot.attributes.emplace_back("FireImmune");
	bot.pressure *= 2.0f;
	}
	*/
	// Parachute does nothing.
	/*
	if (rand_chance(0.1f * chanceMult))
	{
	bot.attributes.emplace_back("Parachute");
	}
	*/

	if (item_class == player_class::demoman)
	{
		if (rand_chance(0.3f * chanceMult))
		{
			float charge_increase = rand_float(1.0f, 15.0f);

			bot_meta.pressure *= ((charge_increase - 1.0f) * 0.03f) + 1.0f;
			if (rand_chance(0.01f * chanceMult))
			{
				charge_increase *= 10000.0f;
				bot_meta.pressure *= 1.5f;
			}

			bot.character_attributes.emplace_back("charge time increased", charge_increase);
		}
	}
	else if (item_class == player_class::pyro)
	{
		if (bot.weapon_restrictions != "MeleeOnly" && bot.weapon_restrictions != "SecondaryOnly")
		{
			if (rand_chance(0.1f * chanceMult))
			{
				float r = rand_float(-10.0f, 10.0f);
				if (rand_chance(0.1f * chanceMult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("airblast pushback scale", r);
			}
			if (rand_chance(0.1f * chanceMult))
			{
				float r = rand_float(-10.0f, 10.0f);
				if (rand_chance(0.1f * chanceMult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("airblast vertical pushback scale", r);
			}
			if (rand_chance(0.1f * chanceMult))
			{
				float r = rand_float(0.1f, 10.0f);
				if (rand_chance(0.1f * chanceMult))
				{
					r *= 100.0f;
				}
				bot.character_attributes.emplace_back("deflection size multiplier", r);
				bot_meta.pressure *= 1.3f;
			}
		}
	}

	if (bot.cl == player_class::engineer)
	{
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.3f, 10.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy building health bonus", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.3f, 5.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry damage bonus", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.1f, 3.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry fire rate increased", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engy sentry radius increased", r);
			bot_meta.pressure *= ((r - 1.0f) * 0.3f) + 1.0f;
		}
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engineer sentry build rate multiplier", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
		}
		if (rand_chance(0.05f * chanceMult))
		{
			float r = rand_float(0.01f, 5.0f);
			if (rand_chance(0.01f * chanceMult))
			{
				r *= 0.01f;
			}
			bot.character_attributes.emplace_back("engineer teleporter build rate multiplier", r);
			bot_meta.pressure /= ((r - 1.0f) * 0.2f) + 1.0f;
		}
	}

	if (rand_chance(0.08f * chanceMult))
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

		if (rand_chance(0.5f))
		{
			float increased_jump_height = rand_float(0.1f, 15.0f);
			bot.character_attributes.emplace_back("increased jump height", increased_jump_height);
			bot.character_attributes.emplace_back("cancel falling damage", 1);
			if (increased_jump_height > 1.0f)
			{
				bot_meta.pressure *= 1.2f;

				if (rand_chance(0.5f))
				{
					bot.items.emplace_back("The B.A.S.E. Jumper");
					bot_meta.pressure *= 1.5f;
				}
			}
		}

		if (rand_chance(0.5f))
		{
			bot.character_attributes.emplace_back("bot custom jump particle", 1);
		}

		if (item_class == player_class::demoman)
		{
			if (rand_chance(0.2f * chanceMult))
			{
				bot.attributes.emplace_back("AirChargeOnly");
				bot_meta.pressure *= 0.9f;
			}
		}
	}
	if (
		//!isGiant &&
		rand_chance(0.15f * chanceMult))
	{
		bot.scale *= rand_float(0.3f, 1.6f);
		bot_meta.pressure /= ((bot.scale - 1.0f) * 0.3f) + 1.0f;
	}

	if (rand_chance(0.1f * chanceMult))
	{
		bot.max_vision_range = 30000.0f;
		if (bot.weapon_restrictions != "MeleeOnly")
		{
			bot_meta.pressure *= 2.0f;
		}
	}
	if (rand_chance(0.1f * chanceMult))
	{
		float fire_rate_bonus = rand_float(0.1f, 2.0f);
		bot.character_attributes.emplace_back("fire rate bonus", fire_rate_bonus);
		bot.character_attributes.emplace_back("clip size bonus", 100000);
		bot_meta.pressure /= ((fire_rate_bonus - 1.0f) * 0.3f) + 1.0f;
	}
	else if (rand_chance(0.1f * chanceMult))
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
		}
		else
		{
			bot.character_attributes.emplace_back("faster reload rate", -1);
			bot_meta.pressure *= 1.4f;
		}
	}
	if (rand_chance(0.1f * chanceMult))
	{
		float damage_bonus_mod;
		if (bot_meta.isGiant)
		{
			damage_bonus_mod = rand_float(0.1f, 3.0f);
		}
		else
		{
			damage_bonus_mod = rand_float(0.1f, 1.0f);
		}
		bot_meta.damage_bonus *= damage_bonus_mod;
	}
	if (rand_chance(0.1f * chanceMult))
	{
		float rad;
		if (bot_meta.isGiant)
		{
			rad = rand_float(0.1f, 3.0f);
		}
		else
		{
			rad = rand_float(0.1f, 1.0f);
		}
		bot.character_attributes.emplace_back("bullets per shot bonus", rad);
		bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
	}
	if (rand_chance(0.1f * chanceMult) && secondary != "Bonk! Atomic Punch" && secondary != "Festive Bonk 2014")
	{
		const float change = rand_float(0.01f, 2.0f);
		bot.character_attributes.emplace_back("effect bar recharge rate increased", change);
		if (change < 1.0f)
		{
			bot_meta.pressure /= ((change - 1.0f) * 0.3f) + 1.0f;
		}
	}
	//if (bot.weapon_restrictions != "MeleeOnly")
	//{
	if (rand_chance(0.1f * chanceMult))
	{
		const float change = rand_float(0.01f, 10.0f);
		bot.character_attributes.emplace_back("projectile speed increased", change);
		bot_meta.pressure *= ((change - 1.0f) * 0.2f) + 1.0f;
	}
	if (rand_chance(0.1f * chanceMult))
	{
		int change = 3;
		while (rand_chance(0.5f) && change < 360)
		{
			change *= 3;
		}
		bot.character_attributes.emplace_back("projectile spread angle penalty", change);
	}
	if (rand_chance(0.05f * chanceMult))
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
	if (rand_chance(0.03f * chanceMult))
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
		bot_meta.pressure *= 2.5f;
	}
	if (rand_chance(0.05f * chanceMult))
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
		bot.character_attributes.emplace_back("head scale", head_size);
	}
	if (rand_chance(0.2f * chanceMult))
	{
		int proj_type = rand_int(1, 20);

		// Fix invalid values.
		switch (proj_type)
		{
		case 7:
			proj_type = 6; // Flare
			break;

		case 9:
		case 10:
		case 15:
		case 16:
			proj_type = 2; // Rocket
			break;
		}

		bot.character_attributes.emplace_back("override projectile type", proj_type);

		if (proj_type == 2 && item_class != player_class::soldier)
		{
			bot_meta.pressure *= 1.5f;
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
	if (rand_chance(0.05f * chanceMult))
	{
		bot.character_attributes.emplace_back("bleeding duration", 5.0f);
		bot_meta.pressure *= 2.0f;
	}
	if (rand_chance(0.04f * chanceMult))
	{
		bot.character_attributes.emplace_back("Set DamageType Ignite", 1);
		// Add some cool fire particles.
		bot.character_attributes.emplace_back("attach particle effect static", 13);
		bot_meta.pressure *= 2.0f;
		if (rand_chance(0.1f * chanceMult))
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
	if (rand_chance(0.05f * chanceMult))
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
			bot_meta.pressure *= 2.0f;
		}
	}
	if (rand_chance(0.05f * chanceMult))
	{
		bot.character_attributes.emplace_back("no self blast dmg", 1);
	}
	if (rand_chance(0.01f))
	{
		bot.character_attributes.emplace_back("melee bounds multiplier", 360.0f);
		bot.character_attributes.emplace_back("melee range multiplier", 30000.0f);
		bot.character_attributes.emplace_back("Blast radius increased", 30000.0f);
		bot_meta.damage_bonus *= 0.01f;
		// Compensate for the massively decreased damage bonus.
		bot_meta.pressure *= 50.0f;
	}
	if (rand_chance(0.02f))
	{
		bot.character_attributes.emplace_back("use large smoke explosion", 1);
		bot_meta.pressure *= 1.2f;
	}
	else if (rand_chance(0.05f * chanceMult))
	{
		float rad = rand_float(0.1f, 5.0f);
		bot.character_attributes.emplace_back("Blast radius increased", rad);
		bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
		if (rad > 1.0f && rand_chance(0.5f))
		{
			bot.character_attributes.emplace_back("use large smoke explosion", 1);
		}
	}
	if (item_class == player_class::soldier || item_class == player_class::sniper || item_class == player_class::pyro)
	{
		if (rand_chance(0.1f))
		{
			const float rad = rand_float(0.1f, 20.0f);
			bot.character_attributes.emplace_back("increase buff duration", rad);
			bot_meta.pressure *= ((rad - 1.0f) * 0.3f) + 1.0f;
		}
	}
	if (rand_chance(0.03f))
	{
		bot.character_attributes.emplace_back("hit self on miss", 1);
	}
	if (rand_chance(0.02f * chanceMult))
	{
		float r = rand_float(-10000.0f, 10000.0f);
		bot.character_attributes.emplace_back("apply z velocity on damage", r);
		bot_meta.pressure *= 2.0f;
	}
	/*
	if (rand_chance(0.05f * chanceMult))
	{
	bot.character_attributes.emplace_back("projectile penetration", 1);
	bot.pressure *= 1.1f;
	}
	*/
	if (rand_chance(0.01f))
	{
		bot.character_attributes.emplace_back("attach particle effect static", rand_int(1, 48));
		bot.health *= 2;
	}

	// Set a random skill level for the bot.
	std::vector<std::string> skills({ "Easy", "Normal", "Hard", "Expert" });
	int skill_index = rand_int(0, skills.size());
	bot.skill = skills.at(skill_index);
	float skill_pressure = (skill_index * 0.4f) + 1.0f;
	bot_meta.pressure *= skill_pressure;

	bot.character_attributes.emplace_back("damage bonus", bot_meta.damage_bonus);
	if (bot_meta.damage_bonus > 0.0f)
	{
		bot_meta.pressure *= bot_meta.damage_bonus;
		//bot_meta.pressure *= bot_meta.damage_bonus * 2.0f;
	}

	std::cout << "Pre-TotalCount loop bot health: " << bot.health << std::endl;

	if (bot.health <= 0)
	{
		throw std::exception("bot_generator::generate_bot exception: Non-positive bot health!");
	}

	std::cout << "Pre-TotalCount loop bot pressure (without health): " << bot_meta.pressure << std::endl;

	if (bot_meta.pressure < 0.0f)
	{
		throw std::exception("bot_generator::generate_bot exception: Negative bot pressure!");
	}

	return bot_meta;
}

void bot_generator::make_bot_into_giant(tfbot_meta& bot_meta)
{
	tfbot& bot = bot_meta.get_bot();
	bot_meta.make_giant();
	bot_meta.isGiant = true;
	bot.scale = -1.0f;
	// Being a giant multiplies the TFBot's health by 15.
	bot.health *= 15;
	// Add some giant-related attributes.
	bot.character_attributes.emplace_back("airblast vulnerability multiplier", rand_float(0.3f, 0.7f));
	bot.character_attributes.emplace_back("damage force reduction", rand_float(0.3f, 0.7f));
	// If the class isn't Scout, incur a move speed penalty...
	if (bot.cl != player_class::scout)
	{
		bot_meta.move_speed_bonus *= 0.5f;
		bot_meta.pressure *= 0.5f;
	}

	// Modify the bot class icon.
	bot.class_icon += "_giant";
	switch (bot.cl)
	{
	case player_class::scout:
		if (bot.class_icon != "scout_bonk_giant" && bot_meta.move_speed_bonus > 1.0f)
		{
			bot.class_icon = "scout_giant_fast";
		}
		break;

	case player_class::soldier:
		if (bot_meta.isAlwaysCrit && bot.class_icon == "soldier_giant")
		{
			bot.class_icon = "soldier_crit";
		}
	}

	// A giant has a chance to be a BOSS!!!
	if (rand_chance(0.1f))
	{
		bot_meta.isBoss = true;
		bot.health *= 5;
		bot_meta.move_speed_bonus *= 0.5;
		bot_meta.pressure *= 0.5;
		bot.attributes.emplace_back("UseBossHealthBar");
		chanceMult *= 4.0f;
		bot.scale = 1.9f;

		bot.character_attributes.emplace_back("attach particle effect static", rand_int(1, 48));

		// Now that you're a boss, choose a special class icon.
		switch (bot.cl)
		{
		case player_class::demoman:
			//if (bot.class_icon == "demo_giant")
			if (bot.weapon_restrictions == "PrimaryOnly")
			{
				bot.class_icon = "demo_bomber";
				bot.items.emplace_back("Prince Tavish's Crown");
			}
			else if (bot.class_icon == "demoknight_giant")
			{
				bot.class_icon = "demoknight_samurai";
			}
			break;

		case player_class::heavyweapons:
			if (bot.weapon_restrictions == "PrimaryOnly")
			{
				bot.class_icon = "heavy_chief";
				bot.items.emplace_back("War Head");
			}
			else if (bot.class_icon == "heavy_gru_giant")
			{
				bot.class_icon = "heavy_urgent";
			}
			break;

		case player_class::scout:
			if (bot.class_icon == "scout_stun_giant")
			{
				bot.class_icon = "scout_stun_giant_armored";
			}
			break;

		case player_class::soldier:
			if (bot.weapon_restrictions == "MeleeOnly")
			{
				bot.class_icon = "soldier_major_crits";
				bot.items.emplace_back("Full Metal Drill Hat");
			}
			else if (bot.weapon_restrictions == "SecondaryOnly")
			{
				bot.class_icon = "soldier_sergeant_crits";
				bot.items.emplace_back("Tyrant's Helm");
			}
			else
			{
				bot.class_icon = "soldier_barrage";
			}
			break;
		}
	}
}