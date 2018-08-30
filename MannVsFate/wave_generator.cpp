#include "wave_generator.h"
#include "rand_util.h"
#include "random_name_generator.h"
#include "mission.h"
#include "tank.h"
#include "currency_manager.h"
#include "pressure_manager.h"
#include "bot_generator.h"
#include "tank_generator.h"
#include "json.hpp"
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <iostream>

const std::string wave_generator::version = "0.4.13";

wave_generator::wave_generator(currency_manager& cm, pressure_manager& pm, bot_generator& botgen, tank_generator& tankgen)
	: mission_currency(cm), wave_pressure(pm), botgen(botgen), tankgen(tankgen),
	current_wave(0),
	respawn_wave_time(2),
	event_popfile(0),
	fixed_respawn_wave_time(false),
	add_sentry_buster_when_damage_dealt_exceeds(3000),
	add_sentry_buster_when_kill_count_exceeds(15),
	can_bots_attack_while_in_spawn_room(false),
	sentry_buster_cooldown(1.0f),
	map_name("mvm_bigrock"),
	mission_name("gen"),
	waves(9),
	max_wavespawns(0),
	max_time(300),
	max_icons(23),
	tank_chance(0.05f),
	max_tfbot_wavespawn_time(120),
	max_tank_wavespawn_time(300),
	use_wacky_sounds(0),
	wacky_sound_vo_ratio(0.1f),
	doombot_enabled(false),
	force_tfbot_hp(nullptr)
{}

void wave_generator::set_map_name(const std::string& in)
{
	map_name = in;
}

void wave_generator::set_mission_name(const std::string& in)
{
	mission_name = in;
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

void wave_generator::set_max_tfbot_wavespawn_time(int in)
{
	max_tfbot_wavespawn_time = in;
}

void wave_generator::set_max_tank_wavespawn_time(int in)
{
	max_tank_wavespawn_time = in;
}

void wave_generator::set_use_wacky_sounds(int in)
{
	use_wacky_sounds = in;
}

void wave_generator::set_wacky_sound_vo_ratio(float in)
{
	wacky_sound_vo_ratio = in;
}

void wave_generator::set_doombot_enabled(bool in)
{
	doombot_enabled = in;
}

void wave_generator::set_force_tfbot_hp(int in)
{
	if (force_tfbot_hp == nullptr)
	{
		force_tfbot_hp = std::make_unique<int>(in);
	}
	else
	{
		*force_tfbot_hp = in;
	}
}

void wave_generator::set_force_tank_hp(int in)
{
	if (force_tank_hp == nullptr)
	{
		force_tank_hp = std::make_unique<int>(in);
	}
	else
	{
		*force_tank_hp = in;
	}
}

void wave_generator::set_force_tank_speed(float in)
{
	if (force_tank_speed == nullptr)
	{
		force_tank_speed = std::make_unique<float>(in);
	}
	else
	{
		*force_tank_speed = in;
	}
}

void wave_generator::generate_mission(int argc, char** argv)
{
	// Important MVM properties differ for each map.

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

	std::ifstream maps_file("data/maps.json");
	if (!maps_file)
	{
		const std::string exstr = "wave_generator::generate_mission exception: Couldn't find maps file \"data/maps.json\".";
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
		wave_pressure.set_bot_path_length(maps_json.at("bot_path_length").get<float>());
	}
	catch (const std::exception&)
	{
		wave_pressure.set_bot_path_length(1.0f);
	}

	try
	{
		botgen.set_scale_mega(maps_json.at("scale_mega").get<float>());
	}
	catch (const std::exception&)
	{
		botgen.set_scale_mega(1.75f);
	}

	try
	{
		botgen.set_scale_doom(maps_json.at("scale_doom").get<float>());
	}
	catch (const std::exception&)
	{
		botgen.set_scale_doom(botgen.get_scale_mega());
	}

	try
	{
		botgen.set_engies_enabled(maps_json.at("engies").get<bool>());
	}
	catch (const std::exception&)
	{
		botgen.set_engies_enabled(true);
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

	try
	{
		tankgen.set_max_tank_speed(maps_json.at("max_tank_speed").get<float>());
	}
	catch (const std::exception&) {}

	list_reader random_sound_reader;
	const std::string file_sounds_standard = "data/sounds.txt";
	const std::string file_sounds_vo = "data/sounds_vo.txt";
	/*
	// Preload the wacky sounds lists.
	if (use_wacky_sounds != 0)
	{
		random_sound_reader.load(file_sounds_standard);
		random_sound_reader.load(file_sounds_vo);
	}
	*/

	std::stringstream filename;
	filename << map_name << '_' << wave_pressure.get_players() << "p_" << mission_name << ".pop";
	const std::string popfile_name = filename.str();

	const std::string tempdir = "temp_";
	const std::string tempext = ".popt";
	writer.popfile_open(tempdir + 'h' + tempext);

	// Write the popfile header.
	writer.write_popfile_header(version, argc, argv);

	writer.block_start("WaveSchedule");

	// Write the WaveSchedule universal options.
	writer.write("StartingCurrency", mission_currency.get_currency());
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

	if (doombot_enabled)
	{
		// Provide some leeway since the Doombot will be running around.
		wave_pressure.multiply_pressure_decay_rate_multiplier(0.8f);
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

		wave_pressure.reset_pressure();
		wave_pressure.calculate_pressure_decay_rate();

		std::cout << "The pressure decay rate is " << wave_pressure.get_pressure_decay_rate() << '.' << std::endl;
		//std::getchar();

		// Cache the recipricol of the pressure decay rate for use in various calculations.
		float recip_pressure_decay_rate = 1 / wave_pressure.get_pressure_decay_rate();

		//std::cout << "recip_pressure_decay_rate: " << recip_pressure_decay_rate << '.' << std::endl;

		// Let's generate the Sentry Buster Mission that coincides with this wave.

		tfbot_meta bot_meta = botgen.generate_bot();
		tfbot& bot = bot_meta.get_bot();

		if (rand_chance(0.95f) && !bot_meta.is_giant)
		{
			botgen.make_bot_into_giant(bot_meta);
		}

		bot.class_icon = "sentry_buster";

		int spawnbot_index = rand_int(0, spawnbots_mega.size());
		std::string location = spawnbots_mega.at(spawnbot_index);

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

		// The last second that the Wavespawns ran for.
		int last_t = 0;

		mission_currency.prepare_for_new_wave();

		std::cout << "Starting actual WaveSpawn generation..." << std::endl;

		// This loop generates all of the WaveSpawns.
		while ((t < max_time || max_time == 0) &&
			(wavespawns.size() < max_wavespawns || max_wavespawns == 0) && 
			(class_icons.size() < max_icons || max_icons == 0) &&
			!mission_currency.has_currency_per_wavespawn_hit_limit())
		{
			if (mission_currency.get_currency_per_wavespawn() != 0)
			{
				// Since the amount of total currency can be determined on the fly in this mode,
				// we can make waves more difficult as they progress!

				wave_pressure.calculate_pressure_decay_rate();

				recip_pressure_decay_rate = 1 / wave_pressure.get_pressure_decay_rate();
			}

			//std::cout << "Generating new wavespawn at t = " << t << '.' << std::endl;

			// The WaveSpawn to generate.
			wavespawn ws;
			// Virtual representation of the WaveSpawn, to be used in pressure calculations.
			virtual_wavespawn vws;

			// Randomly choose whether the WaveSpawn will be a Tank WaveSpawn or not.
			const bool shall_be_tank = rand_chance(tank_chance) && tank_path_starting_points.size() != 0;
			vws.is_tank = shall_be_tank;

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
				
				// Pass work to the tank generator.
				// The resulting tank will have its HP and health set appropriately.
				std::unique_ptr<tank> tnk = tankgen.generate_tank(max_time - t);

				// If the user used certain command line inputs, overwrite the tank's HP and/or speed.
				if (force_tank_hp != nullptr)
				{
					tnk->health = *force_tank_hp;
				}
				if (force_tank_speed != nullptr)
				{
					tnk->speed = *force_tank_speed;
				}

				float effective_pressure = static_cast<float>(tnk->health);
				// How long it should take to kill the theoretical tank.
				float time_to_kill = effective_pressure * recip_pressure_decay_rate;

				float wait_between_spawns = time_to_kill * rand_float(1.0f, 5.0f);
				int max_count = static_cast<int>(floor((time_left) / (wait_between_spawns)));

				/*
				std::cout << "Tank speed / health: " << tnk.speed << " / " << tnk.health << std::endl;
				std::cout << "Tank effective pressure: " << effective_pressure << std::endl;
				std::cout << "Tank time to kill: " << time_to_kill << std::endl;
				*/

				ws.total_count = rand_int(1, max_count + 1);
				ws.wait_before_starting = static_cast<float>(t);
				ws.wait_between_spawns = wait_between_spawns;
				ws.enemy = std::move(tnk);
				// Choose a random path to start on.
				int path_index = rand_int(0, tank_path_starting_points.size());
				ws.location = '\"' + tank_path_starting_points.at(path_index) + '\"';

				vws.effective_pressure = effective_pressure;
				vws.time_to_kill = time_to_kill;

				//std::cout << "Generated Tank." << std::endl;
			}
			else
			{
				// Generate a new TFBot WaveSpawn.

				tfbot_meta bot_meta = botgen.generate_bot();
				tfbot& bot = bot_meta.get_bot();

				// How long it should take to kill the theoretical bot.
				float time_to_kill;

				float effective_pressure;
				float wait_between_spawns;
				int max_count = 0;
				bool has_problem = false;

				// Calculate WaveSpawn data for the TFBot.
				// The following loop makes sure the TFBot doesn't have too much health to handle.

				//std::cout << "Pre-TotalCount loop bot health: " << bot.health << std::endl;
				//std::cout << "Pre-TotalCount loop bot pressure (without health): " << bot_meta.pressure << std::endl;
				//std::cout << "Entering TFBot TotalCount calculation loop..." << std::endl;

				while (max_count == 0 || has_problem)
				{
					has_problem = false;

					effective_pressure = bot_meta.calculate_effective_pressure();
					time_to_kill = effective_pressure * recip_pressure_decay_rate;
					wait_between_spawns = time_to_kill;
					max_count = static_cast<int>(floor(time_left / wait_between_spawns));

					// If the max count is too low, it means the bot may be too strong.
					if (max_count == 0 && bot.health > 25)
					{
						has_problem = true;
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
								has_problem = false;
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
						has_problem = true;
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
					}
					else if (bot_meta.is_boss || bot.health <= 25)
					{
						max_count = 1;
					}
				}

				if (force_tfbot_hp != nullptr)
				{
					bot.health = *force_tfbot_hp;
					effective_pressure = bot_meta.calculate_effective_pressure();
				}

				float wbs_multiplier;
				wbs_multiplier = bot_meta.wait_between_spawns_multiplier;
				// Add some variation to the wait between spawns.
				wbs_multiplier *= rand_float(1.0f, 5.0f);
				wait_between_spawns *= wbs_multiplier;
				// Change the max count too.
				max_count = ceil(static_cast<float>(max_count) / wbs_multiplier);

				// Have the actual pressure be based on the bot's quantity of health.
				effective_pressure = bot.health;

				// Add a little bit of influence from the pressure itself.
				//effective_pressure *= ((bot_meta.pressure - 1.0f) * 0.2f) + 1.0f;

				// Formerly small bots with high health should potentially be made into giants without the additional bonuses.
				if (!bot_meta.is_giant && !bot_meta.perma_small && bot.health >= 1000 && rand_chance(0.7f))
				{
					botgen.make_bot_into_giant_pure(bot_meta);
					botgen.check_bot_scale(bot_meta);
				}
				// If Spies are too large, they'll get stuck in the walls and die when they spawn.
				float constexpr max_spy_scale = 1.2f;
				if (bot.cl == player_class::spy && (bot_meta.is_giant || bot.scale > max_spy_scale))
				{
					bot.scale = max_spy_scale;
				}

				/*
				std::cout << "TotalCount calculation complete." << std::endl;
				std::cout << "Post-TotalCount loop bot health: " << bot.health << std::endl;
				std::cout << "The bot's raw pressure (without health): " << bot_meta.pressure << std::endl;
				std::cout << "TotalCount (pre-write): " << max_count << std::endl;
				std::cout << "WaitBetweenSpawns: " << wait_between_spawns << std::endl;
				std::cout << "TFBot time to kill: " << time_to_kill << std::endl;
				*/

				class_icons.emplace(bot.class_icon);

				//std::cout << "Total class icons so far: " << class_icons.size() << '.' << std::endl;

				// It's time to pass all of this information to the actual WaveSpawn.

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
				ws.enemy = std::make_unique<tfbot>(bot);

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

				vws.effective_pressure = effective_pressure;
				vws.time_to_kill = time_to_kill;

				//std::cout << "Generated TFBot." << std::endl;
			}

			//std::cout << "Wait between spawns: " << ws.wait_between_spawns;
			//std::getchar();

			// Give the WaveSpawn a unique name.
			std::stringstream wsname;
			wsname << "\"wave" << current_wave << '_' << wavespawns.size() + 1 << '\"';
			ws.name = wsname.str();

			// Populate some common properties of virtual WaveSpawns.
			vws.spawns_remaining = ws.total_count - 1;
			vws.wait_between_spawns = ws.wait_between_spawns;
			vws.time_until_next_spawn = ws.wait_between_spawns;

			if (mission_currency.get_currency_per_wavespawn() != 0)
			{
				int additional_currency = mission_currency.calculate_additional_currency_from_wavespawn();
				ws.total_currency += additional_currency;
				vws.currency_per_spawn = additional_currency / ws.total_count;
				//std::cout << "wave_generator: Calculated additional currency from WaveSpawn: " << additional_currency << std::endl;
				//std::getchar();
			}

			if (doombot_enabled)
			{
				ws.support = wavespawn::support_type::limited;
			}

			// Add the virtual WaveSpawn to the pressure manager.
			wave_pressure.add_virtual_wavespawn(vws);
			// Add the actual WaveSpawn to the wavespawns vector.
			wavespawns.emplace_back(ws);

			// Time to do any final work before the next loop iteration (if there is one).

			//std::cout << "wave_generator pressure (prior to pressure loop): " << wave_pressure.get_pressure() << std::endl;

			// Step through time if necessary.
			wave_pressure.step_through_time(t);

			//std::cout << "t = " << t << " (wave " << current_wave << '/' << waves << ')' << std::endl;
		}

		last_t = t;

		std::cout << "Finished generating wave " << current_wave << ". Writing to temporary file..." << std::endl;

		// Finalize the currency total so far now that the wave is over.
		mission_currency.add_currency_from_wave(wavespawns);

		// Allow the bot generator to perform some preparations for the next wave.
		botgen.wave_ended();

		// Time to write the wave to the disk.
		writer.popfile_open(filename_wave.str());

		writer.write_wave_divider(current_wave);
		writer.write_wave_header(wave_start_relay, wave_finished_relay);

		// It's time to write the WaveSpawns.

		if (doombot_enabled)
		{
			// Generate the doombot!
			botgen.set_generating_doombot(true);
			tfbot_meta bot_meta = botgen.generate_bot();
			tfbot& bot = bot_meta.get_bot();
			botgen.set_generating_doombot(false);

			bot_meta.set_base_class_icon("boss");
			//bot.attributes.emplace_back("IgnoreFlag");

			bot.health = static_cast<int>(last_t * wave_pressure.get_pressure_decay_rate() * 0.01f);

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
			//ws.time_to_kill_expires = ws.time_to_kill;
			int spawnbot_index = rand_int(0, spawnbots_doom.size());
			ws.location = spawnbots_doom.at(spawnbot_index);
			ws.enemy = std::make_unique<tfbot>(bot);

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
	std::cout << "Popfile is ready for play." << std::endl;
}