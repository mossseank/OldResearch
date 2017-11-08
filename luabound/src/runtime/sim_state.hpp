/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the SimState class, which loads and populates a lua state with all of the
 *     appropriate functions and definitions needed to define a Rebound simulation.
 */

#ifndef LUABOUND_SIM_STATE_HPP_
#define LUABOUND_SIM_STATE_HPP_

#include "../luabound.hpp"
#include "../sim/distributions.hpp"
#include "../sim/placement.hpp"
#include "../sim/particle.hpp"

class SimState
{
private:
	sol::state *m_lua;

public:
	SimState();
	~SimState();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(SimState)

	bool loadFile(const String& path);

private:
	void prepareLuaState(); // Prepares the lua state with the necessary globals
};

#endif // LUABOUND_SIM_STATE_HPP_