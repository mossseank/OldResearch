/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the Clock class, which can be used to access local time, and get time
 *     formatted strings.
 */

#ifndef LUABOUND_CLOCK_HPP_
#define LUABOUND_CLOCK_HPP_

#include "../luabound.hpp"
#include <ctime>

class Clock
{
public:
	static const String TIMEFMT_SHORT;
	static const String TIMEFMT_LONG;
	static const String TIMEFMT_VERBOSE;
	static const String TIMEFMT_DATE;

private:
	Clock() { }

public:
	static time_t GetRawTime();
	static String GetFormattedTime(const String& format = TIMEFMT_SHORT);
};

#endif // LUABOUND_CLOCK_HPP_