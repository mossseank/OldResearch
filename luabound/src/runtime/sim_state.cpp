/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the SimState class, which loads and populates a lua state with all of the
 *     appropriate functions and definitions needed to define a Rebound simulation.
 */

#include "sim_state.hpp"
#include "simulation.hpp"

// ================================================================================================
SimState::SimState() :
	m_lua{nullptr}
{
	m_lua = new sol::state;
	m_lua->open_libraries(sol::lib::base, sol::lib::math);

	prepareLuaState();
}

// ================================================================================================
SimState::~SimState()
{
	if (m_lua)
		delete m_lua;
	m_lua = nullptr;
}

// ================================================================================================
bool SimState::loadFile(const String& path)
{
	auto result = m_lua->load_file(path);
	if (!result.valid()) {
		sol::error err = result;
		lfatal(strfmt("Lua File Load Error: \"%s\".", err.what()));
		return false;
	}

	sol::protected_function fileFunc = result;
	auto fileRes = fileFunc();
	if (!fileRes.valid()) {
		sol::error err = fileRes;
		lerr(strfmt("Lua File Run Error: \"%s\".", err.what()));
		return false;
	}

	return true;
}

// ================================================================================================
void SimState::prepareLuaState()
{
	sol::state& lua = *m_lua;

	// Register the distributions
	luainterop::RegisterDistributionGlobals(lua);
	// Register the placements
	luainterop::RegisterPlacementGlobals(lua);
	// Register the particle interops
	luainterop::RegisterParticleGlobals(lua);
	// Register the simulation interops
	luainterop::RegisterSimulationGlobals(lua);
}