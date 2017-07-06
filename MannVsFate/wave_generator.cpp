#include "wave_generator.h"
#include "rand_util.h"
#include "random_name_generator.h"
#include "mission.h"
#include "json.hpp"
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <iostream>

const std::string wave_generator::version = "0.4.0";

void wave_generator::set_map_name(const std::string& in)
{
	map_name = in;
}

void wave_generator::set_mission_name(const std::string& in)
{
	mission_name = in;
}

void wave_generator::set_starting_currency(int in)
{
	starting_currency = in;
}

void wave_generator::set_waves(int in)
{
	waves = in;
}

void wave_generator::set_respawn_wave_time(int in)
{
	respawn_wave_time = in;
}

void wave_generator::set_event_popfile(int in)
{
	event_popfile = in;
}

void wave_generator::set_fixed_respawn_wave_time(bool in)
{
	fixed_respawn_wave_time = in;
}

void wave_generator::set_add_sentry_buster_when_damage_dealt_exceeds(int in)
{
	add_sentry_buster_when_damage_dealt_exceeds = in;
}

void wave_generator::set_add_sentry_buster_when_kill_count_exceeds(int in)
{
	add_sentry_buster_when_kill_count_exceeds = in;
}

void wave_generator::set_can_bots_attack_while_in_spawn_room(bool in)
{
	can_bots_attack_while_in_spawn_room = in;
}

void wave_generator::set_sentry_buster_cooldown(float in)
{
	sentry_buster_cooldown = in;
}

void wave_generator::set_players(int in)
{
	players = in;
}

void wave_generator::set_currency_per_wave(int in)
{
	currency_per_wave = in;
}

void wave_generator::set_max_time(int in)
{
	max_time = in;
}

void wave_generator::set_tank_chance(float in)
{
	tank_chance = in;
}

void wave_generator::set_max_icons(int in)
{
	max_icons = in;
}

void wave_generator::set_max_wavespawns(int in)
{
	max_wavespawns = in;
}

void wave_generator::set_possible_classes(const std::vector<player_class>& classes)
{
	botgen.set_possible_classes(classes);
}

void wave_generator::set_max_tfbot_wavespawn_time(int in)
{
	max_tfbot_wavespawn_time = in;
}

void wave_generator::set_max_tank_wavespawn_time(int in)
{
	max_tank_wavespawn_time = in;
}

void wave_generator::set_pressure_decay_rate_multiplier_in_time(float in)
{
	pressure_decay_rate_multiplier_in_time = in;
}

void wave_generator::set_giant_chance(float in)
{
	botgen.set_giant_chance(in);
}

void wave_generator::set_boss_chance(float in)
{
	botgen.set_boss_chance(in);
}

void wave_generator::set_currency_spread(int in)
{
	currency_spread = in;
}

void wave_generator::set_use_wacky_sounds(int in)
{
	use_wacky_sounds = in;
}

void wave_generator::set_wacky_sound_vo_ratio(float in)
{
	wacky_sound_vo_ratio = in;
}

void wave_generator::set_currency_per_wavespawn(int in)
{
	currency_per_wavespawn = in;
}

void wave_generator::set_currency_per_wavespawn_spread(int in)
{
	currency_per_wavespawn_spread = in;
}

void wave_generator::set_currency_per_wavespawn_limit(int in)
{
	currency_per_wavespawn_limit = in;
}

void wave_generator::set_doombot_enabled(bool in)
{
	doombot_enabled = in;
}

void wave_generator::generate_mission(int argc, char** argv)
{
	// Important MVM properties differ for each map.

	// This is the relative length of the map's bot path.
	// This is used to help determine pressure decay.
	// mvm_bigrock is used as a basis, with a length of 1.0f.
	float bot_path_length = 1.0f;
	// This is the name of the wave_start_relay entity.
	std::string wave_start_relay = "wave_start_relay";
	std::string wave_finished_relay = "wave_finished_relay";
	// This is the collection of possible starting points for each generic robot spawn.
	std::vector<std::string> spawnbots;
	// This is the collection of possible starting points for each large robot spawn.
	std::vector<std::string> spawnbots_giant;
	// This is the collection of possible starting points for each boss/mega robot spawn.
	std::vector<std::string> spawnbots_mega;
	// This is the collection of possible starting points for each doom robot spawn.
	std::vector<std::string> spawnbots_doom;
	// This is the collection of starting points for each tank path.
	std::vector<std::string> tank_path_starting_points;
	// Whether enemy engineers can spawn on this map.
	bool engies_enabled = true;
	// The biggest scale that an enemy can possibly have when navigating through a map.
	// Does not take certain spawnbot locations into account.
	float scale_mega;
	// The scale of the doom enemy.
	float scale_doom;

	std::ifstream maps_file("data/maps.json");
	if (!maps_file)
	{
		const std::string exstr = "wave_generator::generate_mission exception: Couldn't find maps file \"data/" + map_name + ".json\".";
		throw std::exception(exstr.c_str());
	}

	// Deserialize the JSON data.
	nlohmann::json maps_json;

	try
	{
		maps_file >> maps_json;
	}
	catch (const std::exception&)
	{
		throw "maps.json exception: JSON parse error.";
	}

	maps_file.close();

	// Retrieve the map data node from the maps JSON.
	try
	{
		maps_json = maps_json.at(map_name);
	}
	catch (const std::exception&)
	{
		const std::string exstr = "maps.json exception: Couldn't find an entry for " + map_name + '!';
		throw std::exception(exstr.c_str());
	}

	// Read the actual map data.

	try
	{
		bot_path_length = maps_json.at("bot_path_length").get<float>();
	}
	catch (const std::exception&)
	{
		bot_path_length = 1.0f;
	}

	try
	{
		scale_mega = maps_json.at("scale_mega").get<float>();
	}
	catch (const std::exception&)
	{
		scale_mega = 1.75f;
	}

	try
	{
		scale_doom = maps_json.at("scale_doom").get<float>();
	}
	catch (const std::exception&)
	{
		scale_doom = scale_mega;
	}

	try
	{
		engies_enabled = maps_json.at("engies").get<bool>();
	}
	catch (const std::exception&)
	{
		engies_enabled = true;
	}

	try
	{
		wave_start_relay = maps_json.at("wave_start_relay").get<std::string>();
	}
	catch (const std::exception&)
	{
		wave_start_relay = "wave_start_relay";
	}

	try
	{
		wave_finished_relay = maps_json.at("wave_finished_relay").get<std::string>();
	}
	catch (const std::exception&)
	{
		wave_finished_relay = "wave_finished_relay";
	}

	try
	{
		spawnbots = maps_json.at("spawnbots").get<std::vector<std::string>>();
	}
	catch (const std::exception&)
	{
		spawnbots.emplace_back("spawnbot");
	}

	try
	{
		spawnbots_giant = maps_json.at("spawngiants").get<std::vector<std::string>>();
	}
	catch (const std::exception&)
	{
		spawnbots_giant = spawnbots;
	}

	try
	{
		spawnbots_mega = maps_json.at("spawnmegas").get<std::vector<std::string>>();
	}
	catch (const std::exception&)
	{
		spawnbots_mega = spawnbots_giant;
	}

	try
	{
		spawnbots_doom = maps_json.at("spawndooms").get<std::vector<std::string>>();
	}
	catch (const std::exception&)
	{
		spawnbots_doom = spawnbots_mega;
	}

	try
	{
		tank_path_starting_points = maps_json.at("spawntanks").get<std::vector<std::string>>();
	}
	catch (const std::exception&)
	{
		// If there are no tank spawn points provided, just assume this map has no tanks.
	}

	botgen.set_engies_enabled(engies_enabled);

	list_reader random_sound_reader;
	const std::string file_sounds_standard = "data/sounds.txt";
	const std::string file_sounds_vo = "data/sounds_vo.txt";
	if (use_wacky_sounds != 0)
	{
		random_sound_reader.load(file_sounds_standard);
		random_sound_reader.load(file_sounds_vo);
	}

	std::stringstream filename;
	filename << map_name << '_' << players << "p_" << mission_name << ".pop";
	const std::string popfile_name = filename.str();

	const std::string tempdir = "temp_";
	const std::string tempext = ".popt";
	writer.popfile_open(tempdir + 'h' + tempext);

	// Write the popfile header.
	writer.write_popfile_header(version, argc, argv);

	writer.block_start("WaveSchedule");

	// Write the WaveSchedule universal options.
	writer.write("StartingCurrency", starting_currency);
	writer.write("RespawnWaveTime", respawn_wave_time);
	if (event_popfile != 0)
	{
		writer.write("EventPopfile", event_popfile);
	}
	if (fixed_respawn_wave_time)
	{
		writer.write("FixedRespawnWaveTime", 1);
	}
	writer.write("AddSentryBusterWhenDamageDealtExceeds", add_sentry_buster_when_damage_dealt_exceeds);
	writer.write("AddSentryBusterWhenKillCountExceeds", add_sentry_buster_when_kill_count_exceeds);
	if (can_bots_attack_while_in_spawn_room)
	{
		writer.write("CanBotsAttackWhileInSpawnRoom", "yes");
	}
	else
	{
		writer.write("CanBotsAttackWhileInSpawnRoom", "no");
	}
	writer.write_blank();

	writer.popfile_close();

	// How much currency the players currently have.
	int current_currency = starting_currency;

	if (doombot_enabled)
	{
		// Provide some leeway since the Doombot will be running around.
		pressure_decay_rate_multiplier *= 0.8f;
	}

	// Generate the actual waves!
	while (current_wave < waves)
	{
		++current_wave;

		std::cout << "Generating wave " << current_wave << '/' << waves << '.' << std::endl;

		std::stringstream filename_wave;
		filename_wave << tempdir << 'w' << current_wave << tempext;
		std::stringstream filename_mission;
		filename_mission << tempdir << 'm' << current_wave << tempext;

		std::cout << "Wrote wave header." << std::endl;

		// How much pressure is currently being placed on the theoretical players of the mission.
		// More pressure means the players are having a harder time.
		// We're assuming our players are disposing of the robot baddies over time,
		// so this pressure value should decay over time if left unattended.
		// If there's too much pressure, we should ease up on the players.
		// If there's too little pressure, we should go harder on the players.
		float pressure = 0.0f;

		// The more currency the players have, the more damage they'll be able to deal.
		// The more damage the players can deal, the quicker they'll be able to dispose of the enemies.
		// The number of players also affects how quickly the robots can be dealt with.
		// Let's have the pressure decay based on this quantity of currency multiplied by the number of players.
		// This decay rate is measured per second.
		// Since this decay rate essentially calculates how powerful and capable the RED team is,
		// it is also used to calculate how powerful the robots should be.
		float pressure_decay_rate = (current_currency * currency_pressure_multiplier + base_pressure_decay_rate)
			* players * pressure_decay_rate_multiplier * bot_path_length;

		//std::cout << "The pressure decay rate is " << pressure_decay_rate << '.' << std::endl;

		// Cache the recipricol of the pressure decay rate for use in various calculations.
		float recip_pressure_decay_rate = 1 / pressure_decay_rate;

		//std::cout << "recip_pressure_decay_rate: " << recip_pressure_decay_rate << '.' << std::endl;

		// Let's generate the Sentry Buster Mission that coincides with this wave.

		botgen.set_pressure_decay_rate(pressure_decay_rate);
		tfbot_meta bot_meta = botgen.generate_bot();
		tfbot& bot = bot_meta.get_bot();

		if (rand_chance(0.95f) && !bot_meta.is_giant)
		{
			botgen.make_bot_into_giant(bot_meta);
		}

		bot.class_icon = "sentry_buster";

		int spawnbot_index = rand_int(0, spawnbots_mega.size());
		std::string location = spawnbots_mega.at(spawnbot_index);

		//const float cooldown_time = std::min(120.0f, bot_meta.calculate_muted_time_to_kill(recip_pressure_decay_rate) * sentry_buster_cooldown);
		const float cooldown_time = std::min(120.0f, bot.health * recip_pressure_decay_rate * 3.5f * sentry_buster_cooldown);

		mission mis;
		mis.objective = "DestroySentries";
		mis.initial_cooldown = 5;
		mis.location = location;
		mis.begin_at_wave = current_wave;
		mis.run_for_this_many_waves = 1;
		mis.cooldown_time = cooldown_time;
		mis.bot = bot;

		writer.popfile_open(filename_mission.str());

		writer.write_mission(mis, spawnbots);

		writer.popfile_close();

		// It's time to start generating the current wave.

		// Our current position in time in the wave as we walk through the wave.
		// We will use this to determine each WaveSpawn's WaitBeforeStarting value.
		int t = 0;
		// A vector of all wavespawns that have been instantiated so far.
		std::vector<wavespawn> wavespawns;
		// An unordered set of all of the icons that are part of the wave so far.
		std::unordered_set<std::string> class_icons;

		// Count the number of active wavespawns.
		// We'll use this to scale pressure based on how many concurrent wavespawns are occurring.
		int active_wavespawns = 0;
		// The last second the Wavespawns ran for.
		int last_t = 0;
		// The approximate amount of currency the players have collected in this wave.
		// This only counts wavespawn currency.
		// We use this to fix roundoff error caused by integer division.
		int approximated_additional_currency = 0;
		// All WaveSpawn currency accumulated so far.
		int wavespawn_currency_so_far = 0;

		std::cout << "Starting actual WaveSpawn generation..." << std::endl;

		// This loop generates all of the WaveSpawns.
		while ((t < max_time || max_time == 0) &&
			(wavespawns.size() < max_wavespawns || max_wavespawns == 0) && 
			(class_icons.size() < max_icons || max_icons == 0) &&
			(wavespawn_currency_so_far < currency_per_wavespawn_limit || currency_per_wavespawn_limit == 0))
		{
			// The rate at which the pressure is decaying when all active wavespawns are taken into consideration.
			/*
			float effective_pressure_decay_rate;

			calculate_effective_pressure_decay_rate(pressure_decay_rate, effective_pressure_decay_rate, wavespawns, t);

			while (effective_pressure_decay_rate > 0 && t < max_time && wavespawns.size() < max_wavespawns && class_icons.size() < max_icons)
			{
				std::cout << "Effective pressure decay rate > 0." << std::endl;
				*/

			if (currency_per_wavespawn != 0)
			{
				// Since the amount of total currency can be determined on the fly in this mode,
				// we can make waves more difficult as they progress!

				pressure_decay_rate = (current_currency * currency_pressure_multiplier + base_pressure_decay_rate)
					* players * pressure_decay_rate_multiplier * bot_path_length;

				recip_pressure_decay_rate = 1 / pressure_decay_rate;
			}

			std::cout << "Generating new wavespawn at t = " << t << '.' << std::endl;

			wavespawn ws;

			// Whether the WaveSpawn will be a Tank WaveSpawn or not.
			const bool shall_be_tank = rand_chance(tank_chance) && tank_path_starting_points.size() != 0;

			// The amount of time the WaveSpawn can fill.
			int time_left;

			int effective_max_time;
			if (shall_be_tank)
			{
				effective_max_time = max_tank_wavespawn_time;
			}
			else
			{
				effective_max_time = max_tfbot_wavespawn_time;
			}

			if (max_time - t > 0)
			{
				time_left = std::min(max_time - t, effective_max_time);
			}
			else
			{
				time_left = effective_max_time;
			}

			if (shall_be_tank)
			{
				// Generate a new tank WaveSpawn.

				class_icons.emplace("tank");

				// 500 is around the maximum amount of speed a tank can have without risking getting stuck,
				// at least on mvm_bigrock.

				// 60.00 * 2.88 * 2.88 = 497.664
				float speed = rand_float(20.0f, 60.0f);
				if (rand_chance(0.2f))
				{
					speed *= 2.88f;
				}
				if (rand_chance(0.2f))
				{
					speed *= 2.88f;
				}

				// The faster the tank moves, the smaller its health should be to account for its speed.
				// Let's make the health both dependent on and inversely proportional to the speed.
				int health = static_cast<int>(pressure_decay_rate * 1500.0f / speed);
				// Round the tank health to the nearest 1000.
				health = static_cast<int>(std::ceil(static_cast<float>(health) / 1000) * 1000);

				float effective_pressure = static_cast<float>(health);
				// How long it should take to kill the theoretical tank.
				float time_to_kill = effective_pressure * recip_pressure_decay_rate;

				//int health = rand_int(1, 100) * 1000;

				//const float speed_factor = 0.4f;
				//float speed_pressure = ((speed - 10.0f) * speed_factor) + 1.0f;

				float wait_between_spawns = time_to_kill * rand_float(1.0f, 5.0f);
				int max_count = static_cast<int>(floor((time_left) / (wait_between_spawns)));

				/*
				while (max_count == 0)
				{
					effective_pressure = health * 0.1f; // 0.2f;
					time_to_kill = effective_pressure * recip_pressure_decay_rate;
					wait_between_spawns = time_to_kill * rand_float(1.0f, 5.0f);
					max_count = static_cast<int>(floor((max_time - t) / (wait_between_spawns)));

					// If the max count is too low, it means the tank may be too strong.
					// Additionally, the tank being too fast with too much health will prove to be problematic.
					if (max_count == 0)
					{
						if (health > 1000)
						{
							health = static_cast<int>(health * 0.9f);
						}
						else
						{
							max_count = 1;
						}
					}
				}
				*/

				std::cout << "Tank speed / health: " << speed << " / " << health << std::endl;
				std::cout << "Tank effective pressure: " << effective_pressure << std::endl;
				std::cout << "Tank time to kill: " << time_to_kill << std::endl;
				//std::getchar();

				// Add the tank WaveSpawn to the wavespawns vector.

				std::stringstream wsname;
				wsname << "\"wave" << current_wave << '_' << wavespawns.size() + 1 << '\"';
				ws.name = wsname.str();
				ws.total_count = rand_int(1, max_count + 1);
				ws.wait_before_starting = static_cast<float>(t);
				ws.wait_between_spawns = wait_between_spawns;
				ws.spawns_remaining = ws.total_count - 1;
				ws.effective_pressure = effective_pressure; // *0.2f;
				ws.time_to_kill = time_to_kill; // *0.2f;
				ws.type_of_spawned = wavespawn::type::tank;
				ws.tnk.speed = speed;
				ws.tnk.health = health;
				ws.time_until_next_spawn = ws.wait_between_spawns;
				ws.time_to_kill_expires = ws.time_to_kill;

				// Choose a random path to start on.
				int path_index = rand_int(0, tank_path_starting_points.size());
				ws.location = '\"' + tank_path_starting_points.at(path_index) + '\"';

				std::cout << "Generated Tank." << std::endl;
			}
			else
			{
				// Generate a new TFBot WaveSpawn.

				botgen.set_pressure_decay_rate(pressure_decay_rate);
				tfbot_meta bot_meta = botgen.generate_bot();
				tfbot& bot = bot_meta.get_bot();

				std::cout << "Pre-TotalCount loop bot health: " << bot.health << std::endl;
				std::cout << "Pre-TotalCount loop bot pressure (without health): " << bot_meta.pressure << std::endl;

				// Calculate WaveSpawn data for the TFBot.
				// The following loop makes sure the TFBot doesn't have too much health to handle.

				std::cout << "Entering TFBot TotalCount calculation loop..." << std::endl;

				// How long it should take to kill the theoretical bot.
				float time_to_kill;
				//float effective_time_to_kill;

				/*
				// Will be 0 if there's no pressure.
				float pressure_compensation = pressure * recip_pressure_decay_rate;
				if (pressure_compensation < 1.0f)
				{
					pressure_compensation = 1.0f;
				}
				else
				{
					pressure_compensation = ((pressure_compensation - 1.0f) * 0.2f) + 1.0f;
					std::cout << "Pressure is nonzero (" << pressure << "). Pressure compensation: " << pressure_compensation << '.' << std::endl;
					std::getchar();
				}
				*/

				float effective_pressure;
				float wait_between_spawns;
				int max_count = 0;

				while (max_count == 0)
				{
					effective_pressure = bot_meta.calculate_effective_pressure();
					time_to_kill = effective_pressure * recip_pressure_decay_rate;
					wait_between_spawns = time_to_kill * rand_float(1.0f, 5.0f); // * pressure_compensation;
					max_count = static_cast<int>(floor(time_left / wait_between_spawns));

					//effective_time_to_kill = effective_pressure / effective_pressure_decay_rate;

					//bool has_pressure_issue = (pressure_compensation > 0.0f && bot.health > pressure_decay_rate * 100.0f / pressure_compensation);

					// If the max count is too low, it means the bot may be too strong.
					if (max_count == 0 && bot.health > 25)
					{
						//max_count = 0;
						// If the wave isn't almost over, keep dwindling the bot's health down.
						if (max_time - t > 20)
						{
							bot.health = static_cast<int>(bot.health * 0.9f);
						}
						// If the wave is almost over...
						else
						{
							// Just let the bot live at its full strength if it's strong, but not TOO strong.
							max_count = static_cast<int>(floor(max_time / wait_between_spawns));
							if (max_count != 0)
							{
								max_count = 1;
							}
							else
							{
								bot.health = static_cast<int>(bot.health * 0.9f);
							}
						}
					}
					else if (time_to_kill < 1.0f)
					{
						//if (rand_chance(0.9f))
						//{
							if (bot_meta.is_giant)
							{
								bot.health *= 2;
							}
							else if (!bot_meta.perma_small)
							{
								botgen.make_bot_into_giant(bot_meta);
							}
							else
							{
								bot.health *= 2;
							}
							max_count = 0;
						//}
					}
					else if (bot_meta.is_boss || bot.health <= 25)
					{
						max_count = 1;
					}
				}

				// Formerly small bots with high health should potentially be made into giants without the additional bonuses.
				if (!bot_meta.is_giant && !bot_meta.perma_small && bot.health >= 1000 && rand_chance(0.7f))
				{
					botgen.make_bot_into_giant_pure(bot_meta);
				}
				// If Spies are too large, they'll get stuck in the walls and die when they spawn.
				float constexpr max_spy_scale = 1.25f;
				if (bot.cl == player_class::spy && (bot_meta.is_giant || bot.scale > max_spy_scale))
				{
					bot.scale = max_spy_scale;
				}

				/*
				// Increase the doombot's health a lot.
				if (bot_meta.is_doom)
				{
					bot.health *= static_cast<int>(max_time * 0.2f);
				}
				*/

				//max_count = ceil(static_cast<float>(max_count) * 0.7f);

				std::cout << "TotalCount calculation complete." << std::endl;
				std::cout << "Post-TotalCount loop bot health: " << bot.health << std::endl;
				std::cout << "The bot's effective pressure (with health): " << effective_pressure << std::endl;
				std::cout << "TotalCount (pre-write): " << max_count << std::endl;
				std::cout << "WaitBetweenSpawns: " << wait_between_spawns << std::endl;
				std::cout << "TFBot time to kill: " << time_to_kill << std::endl;

				class_icons.emplace(bot.class_icon);

				std::cout << "Total class icons so far: " << class_icons.size() << '.' << std::endl;

				// It's time to pass all of this information to the actual wavespawn.

				std::stringstream wsname;
				wsname << "\"wave" << current_wave << '_' << wavespawns.size() + 1 << '\"';
				ws.name = wsname.str();
				if (bot.cl == player_class::engineer || bot.cl == player_class::medic)
				{
					// Reduce the total count of engies and medics.
					max_count = static_cast<int>(std::ceil(static_cast<float>(max_count) * 0.2f));
					ws.total_count = rand_int(1, max_count + 1);
					int at_once_count;
					switch (bot.cl)
					{
					case player_class::engineer:
						at_once_count = 2;
						break;

					case player_class::medic:
						at_once_count = 3;
						break;
					}
					ws.max_active = std::min(at_once_count, ws.total_count);
				}
				else
				{
					ws.total_count = rand_int(1, max_count + 1);
					ws.max_active = std::min(22, ws.total_count);
				}
				ws.wait_before_starting = static_cast<float>(t);
				ws.wait_between_spawns = wait_between_spawns;
				ws.bot = bot;
				if (bot_meta.is_doom)
				{
					ws.effective_pressure = 0.0f;
				}
				else
				{
					ws.effective_pressure = effective_pressure;
					//ws.effective_pressure = bot_meta.calculate_muted_effective_pressure();
					//ws.effective_pressure = bot.health;
				}
				ws.time_to_kill = time_to_kill;
				ws.spawns_remaining = ws.total_count - 1;
				ws.type_of_spawned = wavespawn::type::tfbot;
				ws.time_until_next_spawn = ws.wait_between_spawns;
				ws.time_to_kill_expires = ws.time_to_kill;

				// Decide on the possible locations at which to spawn based on the size of the robot.
				// Larger robots get stuck in some wavespawns, so those wavespawns must be omitted.
				std::vector<std::string>* possible_locations;
				if (bot_meta.is_doom)
				{
					possible_locations = &spawnbots_doom;
				}
				else if (bot_meta.is_boss)
				{
					possible_locations = &spawnbots_mega;
				}
				else if (bot_meta.is_giant || bot.scale > 1.0f)
				{
					possible_locations = &spawnbots_giant;
				}
				else
				{
					possible_locations = &spawnbots;
				}
				int spawnbot_index = rand_int(0, possible_locations->size());
				ws.location = possible_locations->at(spawnbot_index);

				// If applicable, give the WaveSpawn a wacky FirstSpawnWarningSound.
				if (use_wacky_sounds & 1)
				{
					std::string sound;
					if (rand_chance(wacky_sound_vo_ratio))
					{
						sound = random_sound_reader.get_random(file_sounds_vo);
					}
					else
					{
						sound = random_sound_reader.get_random(file_sounds_standard);
					}
					ws.first_spawn_warning_sound = sound;
				}

				std::cout << "Generated TFBot." << std::endl;
			}
			/*
			// Recalculate the effective pressure decay rate.
			calculate_effective_pressure_decay_rate(pressure_decay_rate, effective_pressure_decay_rate, wavespawns, t);
		}
		*/

			if (currency_per_wavespawn != 0)
			{
				int additional_currency = currency_per_wavespawn + rand_int(-currency_per_wavespawn_spread, currency_per_wavespawn_spread);
				// Make sure this Wavespawn's currency doesn't go above the cap.
				if (wavespawn_currency_so_far + additional_currency > currency_per_wavespawn_limit)
				{
					additional_currency = currency_per_wavespawn_limit - wavespawn_currency_so_far;
				}
				ws.total_currency += additional_currency;
				ws.currency_per_spawn = additional_currency / ws.total_count;
				wavespawn_currency_so_far += additional_currency;
				current_currency += additional_currency;
			}

			if (doombot_enabled)
			{
				ws.support = wavespawn::support_type::limited;
			}

			++active_wavespawns;
			pressure += ws.effective_pressure; // *active_wavespawns;

			// Add the wavespawn to the wavespawns vector.
			wavespawns.emplace_back(ws);

			// Time to do any final work before the next loop iteration (if there is one).

			std::cout << "wave_generator pressure (prior to pressure loop): " << pressure << std::endl;

			last_t = t;

			// An empty second is a second that occurs without a single new WaveSpawn.
			// To prevent boredom, this amount of time is capped.
			//int empty_seconds = 0;
			while (pressure > 0.0f
				//&& empty_seconds < 3
				)
			{
				// Increment time.
				++t;
				//++empty_seconds;

				float pressure_increase = 0.0f;
				// Iterate through the wavespawns to update the pressure.
				for (wavespawn& ws : wavespawns)
				{
					// TTKEGTZ: Time To Kill Expires Greater Than 0
					bool ttkegtz = false;
					if (ws.time_to_kill_expires > 0.0f)
					{
						ttkegtz = true;
					}
					ws.time_to_kill_expires -= 1.0f;
					if (ttkegtz && ws.time_to_kill_expires <= 0.0f)
					{
						// A WaveSpawn's theoretical enemy was killed before the next one could spawn.
						// The WaveSpawn has gone quiet for now.
						// This does not mean that the WaveSpawn has finished spawning altogether, however!
						--active_wavespawns;
						ttkegtz = false;
					}

					ws.time_until_next_spawn -= 1.0f;

					while (ws.spawns_remaining != 0 && ws.time_until_next_spawn <= 0.0f)
					{
						/*
						if (ws.spawns_remaining == ws.total_count)
						{
							++active_wavespawns;
						}
						if (ws.spawns_remaining == 0)
						{
							--active_wavespawns;
						}
						else
						{
						*/
						pressure_increase += ws.effective_pressure;
						ws.time_until_next_spawn += ws.wait_between_spawns;

						if (!ttkegtz)
						{
							++active_wavespawns;
						}
						ws.time_to_kill_expires = ws.time_to_kill;
						ttkegtz = true;
						//}
						ws.spawns_remaining -= 1;

						// Since an enemy just died, let's collect its currency.
						current_currency += ws.currency_per_spawn;
						approximated_additional_currency += ws.currency_per_spawn;

						// Since something just spawned, reset the empty seconds.
						//empty_seconds = 0;
						//std::cout << "Empty seconds reset." << std::endl;
					}
				}

				// Modify the pressure.
				pressure += pressure_increase; // *active_wavespawns;
				// Since time has passed, reduce the pressure.
				pressure -= pressure_decay_rate * pressure_decay_rate_multiplier_in_time / (active_wavespawns + 1);
			}

			//++t;

			std::cout << "t = " << t << " (wave " << current_wave << '/' << waves << ')' << std::endl;
		}

		std::cout << "Finished generating wave " << current_wave << ". Writing to popfile..." << std::endl;

		if (currency_per_wave != 0)
		{
			// Calculate the effective currency in this wave by factoring in the currency spread.
			int currency_in_this_wave = currency_per_wave + rand_int(-currency_spread, currency_spread);
			// Make sure the currency in this wave doesn't dip below 0.
			if (currency_in_this_wave < 0)
			{
				currency_in_this_wave = 0;
			}

			// Increment the total currency accrued so far (not counting bonuses).
			current_currency += currency_in_this_wave;

			// Calculate how much currency each wavespawn will have (after the fact).
			int currency_per_wavespawn = currency_in_this_wave / wavespawns.size();

			for (wavespawn& ws : wavespawns)
			{
				ws.total_currency += currency_per_wavespawn;
			}

			// Calculate the roundoff error from integer division.
			int approx_sum_currency_per_wavespawn = currency_per_wavespawn * wavespawns.size();
			int error = currency_per_wave - approx_sum_currency_per_wavespawn;

			// Correct this error by adding some extra currency to the last WaveSpawn.
			wavespawn& ws = wavespawns.back();
			ws.total_currency += error;
		}

		if (approximated_additional_currency != 0)
		{
			// Let's fix the roundoff error of integer division.
			current_currency -= approximated_additional_currency;
			if (currency_per_wavespawn_limit == wavespawn_currency_so_far)
			{
				current_currency += currency_per_wavespawn_limit;
			}
			else
			{
				int true_additional_currency = currency_per_wavespawn * wavespawns.size();
				current_currency += true_additional_currency;
			}
		}

		// Time to write the wave.
		writer.popfile_open(filename_wave.str());

		writer.write_wave_divider(current_wave);
		writer.write_wave_header(wave_start_relay, wave_finished_relay);

		// It's time to write the WaveSpawns.

		if (doombot_enabled)
		{
			// Generate the doombot!
			botgen.set_pressure_decay_rate(pressure_decay_rate);
			botgen.set_generating_doombot(true);
			tfbot_meta bot_meta = botgen.generate_bot();
			tfbot& bot = bot_meta.get_bot();
			botgen.set_generating_doombot(false);

			bot_meta.set_base_class_icon("boss");
			bot.scale = scale_doom;
			//bot.attributes.emplace_back("IgnoreFlag");

			//bot.health *= last_t * recip_pressure_decay_rate * 0.1f;
			bot.health = last_t * recip_pressure_decay_rate * 3500.0f; // 5000.0f // 2500.0f
			//bot.health = last_t * recip_pressure_decay_rate * 500000.0f / bot_meta.pressure;

			/*
			if (!bot_meta.is_always_fire_weapon)
			{
				bot.attributes.emplace_back("AlwaysFireWeapon");
				bot_meta.is_always_fire_weapon = true;
			}
			*/

			wavespawn ws;

			std::stringstream wsname;
			wsname << "\"wave" << current_wave << "_doombot\"";
			ws.name = wsname.str();
			ws.total_count = 1;
			ws.wait_before_starting = 1.0f;
			ws.wait_between_spawns = 1.0f;
			ws.spawns_remaining = 0;
			ws.effective_pressure = 0.0f;
			ws.time_to_kill = 1.0f;
			ws.type_of_spawned = wavespawn::type::tfbot;
			ws.time_until_next_spawn = ws.wait_between_spawns;
			ws.time_to_kill_expires = ws.time_to_kill;
			int spawnbot_index = rand_int(0, spawnbots_doom.size());
			ws.location = spawnbots_doom.at(spawnbot_index);
			ws.bot = bot;

			writer.write_wavespawn(ws, spawnbots);
		}

		// Write all of the standard WaveSpawns.
		for (const wavespawn& ws : wavespawns)
		{
			writer.write_wavespawn(ws, spawnbots);
		}

		// Write randomized sound WaveSpawns each second, if applicable.
		if (use_wacky_sounds & 2)
		{
			int t = 0;
			while (t < last_t)
			{
				writer.block_start("WaveSpawn");
				std::string sound;
				if (rand_chance(wacky_sound_vo_ratio))
				{
					sound = random_sound_reader.get_random(file_sounds_vo);
				}
				else
				{
					sound = random_sound_reader.get_random(file_sounds_standard);
				}
				writer.write("FirstSpawnWarningSound", '\"' + sound + '\"');
				writer.write("WaitBeforeStarting", t);
				writer.write("WaitBetweenSpawns", 1);
				writer.block_end(); // WaveSpawn
				++t;
			}
		}

		writer.block_end(); // Wave

		// Close the current wave file.
		writer.popfile_close();
	}

	std::cout << "Write complete. Concatenating pieces..." << std::endl;

	// Concatenate all of the pieces into a single file!
	writer.popfile_open(popfile_name);

	const std::string hfile = tempdir + 'h' + tempext;
	writer.popfile_copy_write(hfile);
	std::remove(hfile.c_str());

	for (int i = 1; i <= waves; ++i)
	{
		std::stringstream filename_mission;
		filename_mission << tempdir << 'm' << i << tempext;
		writer.popfile_copy_write(filename_mission.str());
		std::remove(filename_mission.str().c_str());
	}
	for (int i = 1; i <= waves; ++i)
	{
		std::stringstream filename_wave;
		filename_wave << tempdir << 'w' << i << tempext;
		writer.popfile_copy_write(filename_wave.str());
		std::remove(filename_wave.str().c_str());
	}

	writer.block_end(); // WaveSchedule
	writer.popfile_close();

	std::cout << "Concatenation complete." << std::endl;
}

/*
void wave_generator::calculate_effective_pressure_decay_rate
(const float& pressure_decay_rate, float& effective_pressure_decay_rate, const std::vector<wavespawn>& wavespawns, const int& t)
{
	effective_pressure_decay_rate = pressure_decay_rate;

	std::cout << wavespawns.size() << " wavespawns generated thus far." << std::endl;

	for (const wavespawn& ws : wavespawns)
	{

		float time_of_last_spawn = ws.wait_before_starting + ws.wait_between_spawns * (ws.total_count - 1);
		float time_of_last_dead = time_of_last_spawn + ws.time_to_kill;

		if (t >= ws.wait_before_starting && t <= time_of_last_dead)
		{
			//std::cout << "Wavespawn is active!" << std::endl;

			float wavespawn_pressure_per_second = ws.effective_pressure / ws.wait_between_spawns;

			//std::cout << "Wavespawn's effective pressure: " << ws.effective_pressure << std::endl;
			//std::cout << "Wavespawn's wait between spawns: " << ws.wait_between_spawns << std::endl;
			//std::cout << "Wavespawn's pressure per second: " << wavespawn_pressure_per_second << std::endl;

			effective_pressure_decay_rate -= wavespawn_pressure_per_second;
		}
	}

	std::cout << "Effective pressure decay rate: " << effective_pressure_decay_rate << std::endl;
}
*/