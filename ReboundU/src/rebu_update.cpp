/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <reboundu.h>
#include <rebu_output.hpp>


// ====================================================================================================================
rebu_error_code_t rebu_update()
{
	OutputManager *output = _rebu_get_output_manager();
	if (output)
		output->update();

	return REBU_ERROR_NONE;
}

// ====================================================================================================================
void rebu_exit()
{
	OutputManager *output = _rebu_get_output_manager();
	if (output) {
		
	}
}