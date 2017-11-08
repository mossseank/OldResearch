/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <rebu_output.hpp>


namespace
{

OutputManager *_outputManager = nullptr;

}


// ====================================================================================================================
OutputManager* _rebu_get_output_manager()
{
	return _outputManager;
}

// ====================================================================================================================
rebu_error_code_t rebu_add_output_file(reb_simulation *sim, const char *path, const char *format, double time)
{
	if (!_outputManager)
		_outputManager = new OutputManager(sim);

	return _outputManager->addOutput(path, format, time);
}

// ====================================================================================================================
rebu_error_code_t rebu_add_terminal_output(reb_simulation *sim, const char *format, double time)
{
	if (!_outputManager)
		_outputManager = new OutputManager(sim);

	return _outputManager->addOutput("", format, time);
}