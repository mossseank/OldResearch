/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <rebu_error.hpp>
#include <cstdarg>
#include <cstring>
#include <cstdio>


namespace
{

size_t _errorCount = 0;
char _lastErrorMessage[1024];

}


// ====================================================================================================================
const char* rebu_get_error_string(rebu_error_code_t err)
{
	switch (err) 
	{
		case REBU_ERROR_NONE: return "No error";
		case REBU_ERROR_INVALID_FORMAT: return "Invalid output format string";
		case REBU_ERROR_FILE_NOT_OPENED: return "Could not open file";
		default: return "Unknown error code";
	}
}

// ====================================================================================================================
const char* rebu_error_code_to_str(rebu_error_code_t err)
{
	switch (err)
	{
		case REBU_ERROR_NONE: return "REBU_ERROR_NONE";
		case REBU_ERROR_INVALID_FORMAT: return "REBU_ERROR_INVALID_FORMAT";
		case REBU_ERROR_FILE_NOT_OPENED: return "REBU_ERROR_FILE_NOT_OPENED";
		default: return "UNKNOWN";
	}
}

// ====================================================================================================================
const char* rebu_get_last_error_message()
{
	static bool copied = false;
	if (_errorCount == 0 && !copied) {
		static const char * const initialMsg = "no error\0";
		memcpy(_lastErrorMessage, initialMsg, strlen(initialMsg) + 1);
		copied = true;
	}

	return _lastErrorMessage;
}

// ====================================================================================================================
void _rebu_set_last_error_message(const char* msg, ...)
{
	va_list argptr;
	va_start(argptr, msg);

	vsnprintf(_lastErrorMessage, 1024, msg, argptr);
	
	va_end(argptr);
	++_errorCount;
}