/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the Clock class, which can be used to access local time, and get time
 *     formatted strings.
 */

#include "clock.hpp"


// ================================================================================================
const String Clock::TIMEFMT_SHORT = "%H:%M:%S";
const String Clock::TIMEFMT_LONG = "%d/%m/%y %H:%M:%S";
const String Clock::TIMEFMT_VERBOSE = "%a %b %d, %G %H:%M:%S";
const String Clock::TIMEFMT_DATE = "%d/%m/%y";


// ================================================================================================
/* static */ time_t Clock::GetRawTime()
{
	time_t rawtime;
	time(&rawtime);
	return rawtime;
}

// ================================================================================================
/* static */ String Clock::GetFormattedTime(const String& format)
{
	time_t rawtime = GetRawTime();
	struct tm * nowtime = localtime(&rawtime);
	char outstr[128];
	if (!strftime(outstr, 128, format.c_str(), nowtime)) {
		return "INVALID_TIME";
	}
	return String(outstr);
}