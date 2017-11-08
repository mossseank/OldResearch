/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the LbdSimulation class, which is the main class for creating and launching
 *    a rebound simulation. It also creates the lua state through SimState interally.
 */

#ifndef LUABOUND_SIMULATION_HPP_
#define LUABOUND_SIMULATION_HPP_

#include "../luabound.hpp"
#include "sim_state.hpp"
#include "../util/cmd_line.hpp"
#include "particle/particle_manager.hpp"
#include "particle/particle_factory.hpp"
#include "output/output_manager.hpp"
#include "../util/timer.hpp"
#include "../plugin/plugins_manager.hpp"


namespace callbacks
{

extern void additionalforces_callback(reb_simulation *sim);
extern void pretimestep_callback(reb_simulation *sim);
extern void posttimestep_callback(reb_simulation *sim);
extern void heartbeat_callback(reb_simulation *sim);
extern int collision_callback(reb_simulation *sim, reb_collision col);

} // namespace callbacks


class LbdSimulation
{
public:
	using PopulateFunctionType = std::function<void()>;

private:
	static LbdSimulation *s_instance;

	reb_simulation *m_sim;
	SimState *m_state;
	String m_simFile;
	bool m_parseError;
	bool m_populateError;

	String m_simName;
	String m_integName;
	double m_simMaxTime;
	int m_integrator;
	sol::protected_function m_populateFunction; // This reference is only valid in the loadFile() function

	ParticleManager *m_pManager;
	ParticleFactory *m_pFactory;
	OutputManager *m_oManager;
	PluginsManager *m_pluginManager;

	int64 m_timestepCount;
	Timer m_wallTimer;

public:
	LbdSimulation(const cmd_line_parameters& params);
	~LbdSimulation();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(LbdSimulation)

	inline String getSimulationName() const { return m_simName; }
	inline String getIntegratorName() const { return m_integName; }
	inline int64 getTimestepCount() const { return m_timestepCount; }
	inline double getElapsedWallTime() const { return m_wallTimer.getElapsed(); }

	bool loadFile();
	bool populateSimulation(sol::table& table);
	void runSimulation();

	bool parseSimulationResults(sol::table& table);
	bool parseConstants(sol::table& constants);
	bool parseIntegrator(sol::table& integ);

	void additionalForcesCallback(reb_simulation *sim);
	void preTimestepCallback(reb_simulation *sim);
	void postTimestepCallback(reb_simulation *sim);
	void heartbeatCallback(reb_simulation *sim);
	int collisionCallback(reb_simulation *sim, reb_collision col);

	inline void flagParseError() { m_parseError = true; }
	inline void flagPopulateError() { m_populateError = true; }

	inline ParticleManager* getManager() { return m_pManager; }
	inline ParticleFactory* getFactory() { return m_pFactory; }
	inline reb_simulation* const getSimulation() { return m_sim; }

	void forceExit();

	inline static LbdSimulation* GetInstance() { return s_instance; }
};


namespace luainterop
{

extern void RegisterSimulationGlobals(sol::state& lua);

} // namespace luainterop

#endif // LUABOUND_SIMULATION_HPP_