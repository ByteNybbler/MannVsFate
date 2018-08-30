// Mann vs Fate Wave Generator
// by MNIK
//
// This is a Team Fortress 2 Mann vs Machine random mission generator.
// Recommended to be played with a bomb respawn plugin setting the bomb respawn to 15 seconds.

#include "bot_generator.h"
#include "tank_generator.h"
#include "wave_generator.h"
#include "cosmetics_generator.h"
#include "currency_manager.h"
#include "pressure_manager.h"
#include "json_reader_item_category.h"
#include <iostream>
#include <string>

int toint(const char* in)
{
	return std::stoi(in);
}

float tofloat(const char* in)
{
	return std::stof(in);
}

int main(int argc, char** argv)
{
	try
	{
		// Instantiate the popfile generator and all of its dependencies.
		currency_manager cm;
		pressure_manager pm(cm);
		json_reader_item_category jric;
		cosmetics_generator cosgen(jric.read());
		bot_generator botgen(pm, cosgen);
		tank_generator tankgen(pm);
		wave_generator gen(cm, pm, botgen, tankgen);

		// Interpret command line arguments.
		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
			{
				if (std::strcmp(argv[i], "-map") == 0)
				{
					++i;
					gen.set_map_name(argv[i]);
					continue;
				}
				if (std::strcmp(argv[i], "-name") == 0)
				{
					++i;
					gen.set_mission_name(argv[i]);
					continue;
				}
				if (std::strcmp(argv[i], "-startingcurrency") == 0)
				{
					++i;
					cm.set_currency(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-waves") == 0)
				{
					++i;
					gen.set_waves(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-respawnwavetime") == 0)
				{
					++i;
					gen.set_respawn_wave_time(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-halloween") == 0)
				{
					gen.set_event_popfile(1);
					continue;
				}
				if (std::strcmp(argv[i], "-fixedrespawnwavetime") == 0)
				{
					gen.set_fixed_respawn_wave_time(true);
					continue;
				}
				if (std::strcmp(argv[i], "-busterdamagethreshold") == 0)
				{
					++i;
					gen.set_add_sentry_buster_when_damage_dealt_exceeds(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-busterkillthreshold") == 0)
				{
					++i;
					gen.set_add_sentry_buster_when_kill_count_exceeds(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-botspawnattack") == 0)
				{
					gen.set_can_bots_attack_while_in_spawn_room(true);
					continue;
				}
				if (std::strcmp(argv[i], "-bustercooldown") == 0)
				{
					++i;
					gen.set_sentry_buster_cooldown(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-players") == 0)
				{
					++i;
					pm.set_players(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wavecurrency") == 0)
				{
					++i;
					cm.set_currency_per_wave(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-time") == 0)
				{
					++i;
					gen.set_max_time(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-tankchance") == 0)
				{
					++i;
					gen.set_tank_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-icons") == 0)
				{
					++i;
					gen.set_max_icons(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wavespawns") == 0)
				{
					++i;
					gen.set_max_wavespawns(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-classes") == 0)
				{
					++i;
					const int flags = toint(argv[i]);
					std::vector<player_class> possible_classes;

					if (flags & 1)
					{
						possible_classes.emplace_back(player_class::scout);
					}
					if (flags & 2)
					{
						possible_classes.emplace_back(player_class::soldier);
					}
					if (flags & 4)
					{
						possible_classes.emplace_back(player_class::pyro);
					}
					if (flags & 8)
					{
						possible_classes.emplace_back(player_class::demoman);
					}
					if (flags & 16)
					{
						possible_classes.emplace_back(player_class::heavyweapons);
					}
					if (flags & 32)
					{
						possible_classes.emplace_back(player_class::engineer);
					}
					if (flags & 64)
					{
						possible_classes.emplace_back(player_class::medic);
					}
					if (flags & 128)
					{
						possible_classes.emplace_back(player_class::sniper);
					}
					if (flags & 256)
					{
						possible_classes.emplace_back(player_class::spy);
					}

					botgen.set_possible_classes(possible_classes);

					continue;
				}
				if (std::strcmp(argv[i], "-botmaxtime") == 0)
				{
					++i;
					gen.set_max_tfbot_wavespawn_time(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-tankmaxtime") == 0)
				{
					++i;
					gen.set_max_tank_wavespawn_time(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-difficulty") == 0)
				{
					++i;
					pm.set_pressure_decay_rate_multiplier_in_time(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-giantchance") == 0)
				{
					++i;
					botgen.set_giant_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-bosschance") == 0)
				{
					++i;
					botgen.set_boss_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-giantchanceincrease") == 0)
				{
					++i;
					botgen.set_giant_chance_increase(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-bosschanceincrease") == 0)
				{
					++i;
					botgen.set_boss_chance_increase(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-currencyspread") == 0)
				{
					++i;
					cm.set_currency_per_wave_spread(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wackysounds") == 0)
				{
					++i;
					gen.set_use_wacky_sounds(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wackyvoratio") == 0)
				{
					++i;
					gen.set_wacky_sound_vo_ratio(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wavespawncurrency") == 0)
				{
					++i;
					cm.set_currency_per_wavespawn(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wavespawncurrencyspread") == 0)
				{
					++i;
					cm.set_currency_per_wavespawn_spread(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-wavespawncurrencylimit") == 0)
				{
					++i;
					cm.set_currency_per_wavespawn_limit(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-doombot") == 0)
				{
					gen.set_doombot_enabled(true);
					continue;
				}
				if (std::strcmp(argv[i], "-cosmetics") == 0)
				{
					botgen.set_give_bots_cosmetics(true);
					continue;
				}
				if (std::strcmp(argv[i], "-bothp") == 0)
				{
					++i;
					gen.set_force_tfbot_hp(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-tankhp") == 0)
				{
					++i;
					gen.set_force_tank_hp(toint(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-tankspeed") == 0)
				{
					++i;
					gen.set_force_tank_speed(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-firechance") == 0)
				{
					++i;
					botgen.set_fire_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-bleedchance") == 0)
				{
					++i;
					botgen.set_bleed_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-bleedforall") == 0)
				{
					botgen.set_nonbosses_can_get_bleed(true);
					continue;
				}
				if (std::strcmp(argv[i], "-botscalechance") == 0)
				{
					++i;
					botgen.set_bot_scale_chance(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-minimumbotscale") == 0)
				{
					++i;
					botgen.set_minimum_bot_scale(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-maximumbotscale") == 0)
				{
					++i;
					botgen.set_maximum_bot_scale(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-minimumgiantscale") == 0)
				{
					++i;
					botgen.set_minimum_giant_scale(tofloat(argv[i]));
					continue;
				}
				if (std::strcmp(argv[i], "-forcebotscale") == 0)
				{
					++i;
					botgen.set_force_bot_scale(tofloat(argv[i]));
					continue;
				}
			}
		}

		// Generate the actual mission.
		gen.generate_mission(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "Press any key to terminate the program." << std::endl;
		std::getchar();
	}
	return 0;
}