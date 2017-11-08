/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the Timer class, which can be used to measure elapsed wall time.
 */

#ifndef LUABOUND_TIMER_HPP_
#define LUABOUND_TIMER_HPP_

#include "../luabound.hpp"

// All time measurements are given in decimal seconds
class Timer
{
private:
	uint64 m_start;
	uint64 m_pause;
	bool m_started;
	bool m_paused;

public:
	Timer(bool start = false) :
		m_start{0}, m_pause{0}, m_started{start}, m_paused{false}
	{ 
		if (start)
			this->start();
	}

	inline bool IsStarted() const { return m_started; }
	inline bool IsStopped() const { return !m_started; }
	inline bool IsPaused() const { return m_paused; }
	inline bool IsActive() const { return !m_paused && m_started; }

	void pause();
	void resume();
	void stop();
	void start();
	void reset();

	double getElapsed() const; // In seconds

	static double GetResolution();
};

#endif // LUABOUND_TIMER_HPP_