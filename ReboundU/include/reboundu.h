/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#pragma once
#define REBOUNDU_H_


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
 * Since `restrict` is a C99 keyword only, it is not core to C++ and is the main source of the compatibility issues
 * with using the C API in C++. However, most C++ compilers do support the `__restrict__` extension keyword, which is
 * functionally equivalent. So a simple redefinition is enough to fix this issue. This should work on all major 
 * compilers, but please report if you come across one that does not support this.
 */
#define restrict __restrict__

#include "rebound.h"


// ReboundU error codes
typedef enum
{
	REBU_ERROR_NONE,
	REBU_ERROR_INVALID_FORMAT,
	REBU_ERROR_FILE_NOT_OPENED
} rebu_error_code_t;


// Add an output file to a simulation with the given format
extern rebu_error_code_t rebu_add_output_file(reb_simulation *sim, const char *path, const char *format, double time);

// Add terminal output to a simulation with the given format
extern rebu_error_code_t rebu_add_terminal_output(reb_simulation *sim, const char *format, double time);


// Allow ReboundU to run internal updates, should be called at the end of each heartbeat callback
extern rebu_error_code_t rebu_update();

// Allow ReboundU to close internal handles and free memory, should be called before freeing simulation
extern void rebu_exit();


// Get a descriptive string for an error code
extern const char* rebu_get_error_string(rebu_error_code_t err);

// Get the string version of an error code
extern const char* rebu_error_code_to_str(rebu_error_code_t err);

// Get a string describing the last error
extern const char* rebu_get_last_error_message();


#ifdef __cplusplus
}
#endif // __cplusplus