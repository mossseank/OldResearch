/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the Timer class, which can be used to measure elapsed wall time.
 */

#include "timer.hpp"
#ifdef LUABOUND_PLATFORM_MACOS
#	include <mach/mach_time.h>
#else
#	include <time.h>
#endif // LUABOUND_PLATFORM_MACOS

namespace 
{

// Given in nanoseconds
uint64 _getClockResolution()
{
#ifdef LUABOUND_PLATFORM_MACOS
	// Macs do not support clock_gettime
	// Also, there is no way in Darwin to query for minimum clock resolution, so we have to run this
	//     disgusting hacky fix to try to estimate the resolution.
	static mach_timebase_info_data_t freq = {0, 0};
	static uint64 resolution = 0;
	if (freq.denom == 0) {
		mach_timebase_info(&freq);

		// Make initial guess
		uint64 stime = mach_absolute_time();
		uint64 etime = mach_absolute_time();
		uint64 minres = etime - stime;

		// 10 is an arbitrary count, check multiple times to make sure we are not getting caught by
		//     a context swap, or cache effects
		for (int i = 0; i < 10; ++i) {
			stime = mach_absolute_time();
			etime = mach_absolute_time();

			uint64 candidate = etime - stime;
			if (candidate < minres)
				minres = candidate;
		}

		if (minres == 0) {
			// Either the resolution is <1ns, or something is really high
			// Assume 1ns
			minres = 1;
		}

		resolution = minres;
	}

	return resolution;
#else
	static uint64 resolution = 0;
	if (resolution == 0) {
		timespec restime;
		clock_getres(CLOCK_MONOTONIC, &restime);

		uint64 nano1 = static_cast<uint64>(restime.tv_sec * 1000000000);
		uint64 nano2 = static_cast<uint64>(restime.tv_nsec);
		resolution = nano1 + nano2;
	}

	return resolution;
#endif // LUABOUND_PLATFORM_MACOS
}

// Given in nanoseconds
uint64 _getClockTimestamp()
{
#ifdef LUABOUND_PLATFORM_MACOS
	// Macs do not support clock_gettime
	static mach_timebase_info_data_t freq = {0, 0};
	if (freq.denom == 0)
		mach_timebase_info(&freq);

	uint64 nano = mach_absolute_time() * freq.numer / freq.denom;
	return nano;
#else
	timespec currtime;
	clock_gettime(CLOCK_MONOTONIC, &currtime);

	uint64 nano1 = static_cast<uint64>(currtime.tv_sec * 1000000000);
	uint64 nano2 = static_cast<uint64>(currtime.tv_nsec);
	return (nano1 + nano2);
#endif // LUABOUND_PLATFORM_MACOS
}

} // namespace 


// ================================================================================================
void Timer::pause()
{
	if (m_paused || !m_started)
		return;

	m_paused = true;
	m_pause = _getClockTimestamp();
}

// ================================================================================================
void Timer::resume()
{
	if (!m_paused)
		return;

	m_paused = false;
	m_start += (_getClockTimestamp() - m_pause);
}

// ================================================================================================
void Timer::stop()
{
	m_started = false;
}

// ================================================================================================
void Timer::start()
{
	if (m_started)
		return;

	m_started = true;
	m_paused = false;
	m_start = _getClockTimestamp();
}

// ================================================================================================
void Timer::reset()
{
	m_paused = false;
	m_start = _getClockTimestamp();
}

// ================================================================================================
double Timer::getElapsed() const
{
	if (!m_started)
		return 0;

	if (m_paused)
		return (m_pause - m_start) / 1e9;
	
	return (_getClockTimestamp() - m_start) / 1e9;
}

// ================================================================================================
/* static */ double Timer::GetResolution()
{
	static const uint64 savedNanoseconds = _getClockResolution();
	return savedNanoseconds / 1e9;
}