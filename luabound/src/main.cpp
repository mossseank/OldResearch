/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file is the entry point for the luabound application.
 */

#include "luabound.hpp"
#include "runtime/simulation.hpp"
#include "util/cmd_line.hpp"

void initialize_random();

int main(int argc, char **argv)
{
	// Temporary fix, will eventually allow simulations to define seeds
	initialize_random();

	std::cout << "Luabound version " << LUABOUND_VERSION 
			  << " (Rebound: " << reb_version_str << ", "
			  << "Reboundx: " << "N/A" << ")" << std::endl;

	cmd_line_parameters params;
	parse_command_line(argc, argv, params);

	LbdSimulation sim(params);
	if (!sim.loadFile()) {
		lfatal("Could not load simulation script file. Check output for details.");
		return -1;
	}

	linfo(strfmt("Running simulation '%s'.", sim.getSimulationName().c_str()));

	sim.runSimulation();

	return 0;
}

void initialize_random()
{
	const size_t BYTECOUNT = 4;
	char data[BYTECOUNT] = {0};
	FILE *fp;
	fp = fopen("/dev/urandom", "r");
	if (fread(&data, 1, BYTECOUNT, fp)); // Gets rid of GCC compiler warning for unused return value
	fclose(fp);

	srand(*reinterpret_cast<uint32*>(data));
}