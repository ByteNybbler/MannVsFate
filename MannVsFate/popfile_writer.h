#ifndef POPFILE_WRITER_H
#define POPFILE_WRITER_H

#include "wavespawn.h"
#include "mission.h"
#include <fstream>

// A class for writing to a popfile.
class popfile_writer
{
public:
	// Constructor.
	popfile_writer(const std::string& filename);

	// -=- File functions -=-

	// Open a popfile for writing.
	void popfile_open(const std::string& filename);
	// Close the current popfile.
	void popfile_close();
	// Copy the contents of another file into the current file.
	void popfile_copy_write(const std::string& filename);

	// -=- Elemental writing functions -=-

	// Write a number of indents based on the indentation level.
	void write_indents();
	// Writes a line to the popfile, taking the indentation level into account.
	void write(const std::string& str);
	// Write a string followed by a space followed by an integer.
	void write(const std::string& str, int number);
	// Write a string followed by a space followed by a float.
	void write(const std::string& str, float number);
	// Write a string followed by a space followed by another string.
	void write(const std::string& str1, const std::string& str2);
	// Writes an empty line. It just writes the newline character.
	void write_blank();
	// Starts a block with { and indents. Prior to the {, str is on its own line.
	void block_start(const std::string& str);
	// Ends a block with } and unindents.
	void block_end();

	// -=- Writing functions for mundane things -=-

	// Write the popfile header.
	void write_popfile_header(const std::string& generator_version, int argc, char** argv);
	// Write a "fancy" divider between waves.
	void write_wave_divider(int wave_number);
	// Write a wave header.
	void write_wave_header(const std::string& wave_start_relay, const std::string& wave_finished_relay);

	// -=- Writing functions for compound objects -=-

	// Writes data for one TFBot.
	void write_tfbot(const tfbot& bot, const std::vector<std::string>& spawnbots);
	// Writes data for one WaveSpawn.
	void write_wavespawn(const wavespawn& ws, const std::vector<std::string>& spawnbots);
	// Writes data for one Mission.
	void write_mission(const mission& mis, const std::vector<std::string>& spawnbots);

private:
	// The popfile being written to.
	std::ofstream popfile;
	// The current indentation level.
	int indent = 0;
};

#endif // POPFILE_WRITER_H