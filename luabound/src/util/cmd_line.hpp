/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares structures and functions related to parsing command line arguments.
 */

#ifndef LUABOUND_CMD_LINE_HPP_
#define LUABOUND_CMD_LINE_HPP_

#include "../luabound.hpp"

struct cmd_line_parameters
{
public:
	String scriptFile; // The script to load the simulation from

	cmd_line_parameters() :
		scriptFile{"./simulation.lua"}
	{ }
};

void parse_command_line(int argc, char **argv, cmd_line_parameters& params);

#endif // LUABOUND_CMD_LINE_HPP_