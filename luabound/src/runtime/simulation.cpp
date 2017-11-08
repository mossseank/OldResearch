/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the LbdSimulation class, which is the main class for creating and launching
 *    a rebound simulation. It also creates the lua state through SimState interally.
 */

#include "simulation.hpp"
#include "integrator_parser.hpp"

namespace
{

int _parseIntegratorString(reb_simulation *sim, const String& str)
{
	if (str == "ias15")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_IAS15);
	else if (str == "whfast")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_WHFAST);
	else if (str == "sei")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_SEI);
	else if (str == "leapfrog")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_LEAPFROG);
	else if (str == "hermes")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_HERMES);
	else if (str == "janus")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_JANUS);
	else if (str == "mercurius")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_MERCURIUS);
	else if (str == "none")
		return (sim->integrator = reb_simulation::REB_INTEGRATOR_NONE);
	else
		return -1;
}

bool _validateParticleName(const String& str)
{
	for (const auto& c : str) {
		if (!(std::isalnum(c) || (c == '_')))
			return false;
	}
	return true;
}

} // namespace 

/* static */ LbdSimulation* LbdSimulation::s_instance = nullptr;

// ================================================================================================
LbdSimulation::LbdSimulation(const cmd_line_parameters& params) :
	m_sim{nullptr},
	m_state{nullptr},
	m_simFile{""},
	m_parseError{false},
	m_populateError{false},
	m_simName{""},
	m_integName{""},
	m_simMaxTime{INFINITY},
	m_integrator{reb_simulation::REB_INTEGRATOR_NONE},
	m_pManager{nullptr},
	m_pFactory{nullptr},
	m_oManager{nullptr},
	m_pluginManager{nullptr},
	m_timestepCount{0},
	m_wallTimer{false}
{
	s_instance = this;

	m_state = new SimState;
	m_simFile = params.scriptFile;

	m_sim = reb_create_simulation();
	m_pManager = new ParticleManager(m_sim);
	m_pFactory = new ParticleFactory(m_sim);
	m_oManager = new OutputManager(this);
	m_pluginManager = new PluginsManager;
}

// ================================================================================================
LbdSimulation::~LbdSimulation()
{
	if (m_sim)
		reb_free_simulation(m_sim);
	if (m_state)
		delete m_state;
	if (m_pManager)
		delete m_pManager;
	if (m_pFactory)
		delete m_pFactory;
	if (m_oManager)
		delete m_oManager;
	if (m_pluginManager)
		delete m_pluginManager;
}

// ================================================================================================
bool LbdSimulation::loadFile()
{
	linfo(strfmt("Opening simulation file '%s'.", m_simFile.c_str()));
	
	if (!m_state->loadFile(m_simFile)) { // Check first for load or run errors
		return false;
	}
	if (m_parseError) { // Now check for errors in the simulation table structure
		lerr(strfmt("The argument provided to new_simulation in file '%s' was incorrectly structured, "
			"or otherwise missing formatting or crucial information.", m_simFile.c_str()));
		return false;
	}
	if (m_populateError) {
		lerr(strfmt("Could not populate the simulation in file '%s' with particles.", m_simName.c_str()));
		return false;
	}

	return true;
}

// ================================================================================================
bool LbdSimulation::parseSimulationResults(sol::table& table)
{
	String header = strfmt("=============== PARSE SIMULATION ('%s') ===============", m_simFile.c_str());
	linfo(header);
	lsetPrefix("  ");

	// ===== Simulation Name =====
	sol::object nameObj;
	if ((nameObj = table["name"]) == sol::nil) {
		lerr("A name was not provided for the simulation.");
		return false;
	}
	else {
		m_simName = nameObj.as<String>();
		linfo(strfmt("Loading new simulation with name '%s'.", m_simName.c_str()));
	}

	// ===== Simulation Constants =====
	sol::object constantsTableObj;
	if ((constantsTableObj = table["constants"]) == sol::nil) {
		lerr("A table of constants was not provided for the simulation.");
		return false;
	}
	else if (!constantsTableObj.is<sol::table>()) {
		lerr("The simulation 'constants' entry was not a table.");
		return false;
	}
	else {
		sol::table constantsTable = constantsTableObj.as<sol::table>();
		if (!parseConstants(constantsTable)) {
			return false;
		}
		linfo(strfmt("Loaded constants for simulation '%s'.", m_simName.c_str()));
	}

	// ===== Simulation Plugins =====
	sol::object pluginsTableObj;
	if ((pluginsTableObj = table["plugins"]) == sol::nil) {
		linfo("No plugins registered for this simulation.");
	}
	else {
		if (!pluginsTableObj.is<sol::table>()) {
			lerr("The simulation plugins must be given as a list.");
			return false;
		}

		sol::table plugins = pluginsTableObj.as<sol::table>();
		if (!m_pluginManager->loadPlugins(plugins)) {
			return false;
		}
		linfo(strfmt("Loaded %d plugin(s) for simulation.", m_pluginManager->getPluginCount()));
	}

	// ===== Simulation Integrator =====
	sol::object integTableObj;
	if ((integTableObj = table["integrator"]) == sol::nil) {
		lerr("Integrator settings were not provided for the simulation.");
		return false;
	}
	else if (!integTableObj.is<sol::table>()) {
		lerr("The simulation 'integrator' entry was not a table.");
		return false;
	}
	else {
		sol::table integTable = integTableObj.as<sol::table>();
		if (!parseIntegrator(integTable)) {
			return false;
		}
		linfo(strfmt("Loaded integrator settings for simulation '%s'.", m_simName.c_str()));
	}

	// ===== Populate Function =====
	sol::object populateFuncObj;
	if ((populateFuncObj = table["populate"]) == sol::nil) {
		lerr("No populate() function was provided for the simulation.");
		return false;
	}
	else if (!populateFuncObj.is<PopulateFunctionType>()) {
		lerr("The function signature for populate must take no arguments, and return no values.");
		return false;
	}
	else {
		m_populateFunction = populateFuncObj.as<sol::protected_function>();
		linfo("Loaded populate() function for simulation.");
	}

	// ===== File Output Table =====
	sol::object outputTableObj;
	if ((outputTableObj = table["output"]) == sol::nil) {
		lerr("File output settings were not provided for the simulation.");
		return false;
	}
	else if (!outputTableObj.is<sol::table>()) {
		lerr("The simulation 'output' entry was not a table.");
		return false;
	}
	else {
		sol::table outputTable = outputTableObj.as<sol::table>();
		if (!m_oManager->loadOutput(outputTable)) {
			return false;
		}
		linfo("Loaded file output settings.");
	}

	lsetPrefix("");
	linfo(String(header.length(), '='));

	return true;
}

// ================================================================================================
void LbdSimulation::runSimulation()
{
	m_sim->additional_forces = callbacks::additionalforces_callback;
	m_sim->pre_timestep_modifications = callbacks::pretimestep_callback;
	m_sim->post_timestep_modifications = callbacks::posttimestep_callback;
	m_sim->heartbeat = callbacks::heartbeat_callback;
	m_sim->collision_resolve = callbacks::collision_callback;

	reb_move_to_com(m_sim);
	m_wallTimer.start();
	reb_integrate(m_sim, m_simMaxTime);

	m_pluginManager->shutdown(m_sim);
}

// ================================================================================================
bool LbdSimulation::parseConstants(sol::table& constants)
{
	sol::object cnst;

	// ===== G =====
	if ((cnst = constants["G"]) != sol::nil) {
		if (cnst.get_type() == sol::type::number) {
			m_sim->G = cnst.as<double>();
		}
		else {
			lerr("The value for the constant 'G' must be specified as a number.");
			return false;
		}
	}
	// ===== Simulation Max Time =====
	if ((cnst = constants["max_time"]) != sol::nil) {
		if (cnst.get_type() == sol::type::number) {
			m_simMaxTime = cnst.as<double>();
		}
		else if (cnst.get_type() == sol::type::string) {
			String timestr = cnst.as<String>();
			if (timestr == "inf") {
				m_simMaxTime = INFINITY;
			}
			else {
				lerr(strfmt("Invalid max_time string \"%s\". The only valid string value for max_time is \"inf\". ",
					timestr.c_str()));
				return false;
			}
			lwarn("This simulation is set to run without a max time. Is this what you wanted?");
		}
		else {
			lerr("The value for simulation `max_time` must be specified as a number or \"inf\".");
			return false;
		}
	}

	return true;
}

// ================================================================================================
bool LbdSimulation::parseIntegrator(sol::table& integ)
{
	sol::object entry;
	String integStr;

	// ===== Integrator Name =====
	if ((entry = integ["name"]) == sol::nil) {
		lerr("A integrator name was not specified for the simulation.");
		return false;
	}
	else if (entry.get_type() != sol::type::string) {
		lerr("The integrator type must be specified as a string.");
		return false;
	}
	else {
		m_integName = integStr = entry.as<String>();
		m_integrator = _parseIntegratorString(m_sim, integStr);
		if (m_integrator == -1) {
			lerr("The integrator type string was not a valid value. It must be one of: "
				"'ias15', 'whfast', 'sei', 'leapfrog', 'hermes', 'whfasthelio', or 'none'.");
			return false;
		}

		String iSU = integStr;
		for (char& c : iSU)
			c = std::toupper(c);
		linfo(strfmt("Simulation is using integrator %s.", iSU.c_str()));
	}
	
	// ===== Individual Integrator Parsing =====
	switch (m_integrator) {
		case reb_simulation::REB_INTEGRATOR_IAS15: return integ_parse::IAS15(integ, &(m_sim->ri_ias15));
		default: {
			lerr(strfmt("Parameter parsing for integrator %s not implemented yet.", integStr.c_str()));
			return false;
		}
	}
}

// ================================================================================================
bool LbdSimulation::populateSimulation(sol::table& table)
{
	String header = strfmt("============= POPULATE SIMULATION ('%s') ==============", m_simFile.c_str());
	linfo(header);
	lsetPrefix("  ");

	bool good = true;
	auto result = m_populateFunction();
	if (!result.valid()) {
		sol::error err = result;
		lerr(strfmt("Lua error in populate(): \"%s\".", err.what()));
		good = false;
	}

	linfo(strfmt("Simulation populated with %d particles.", m_sim->N));

	lsetPrefix("");
	linfo(String(header.length(), '='));

	m_pluginManager->startup(m_sim);

	m_populateFunction = sol::function{}; // Clear reference to prevent a segfault when deconstructed
	return good;
}

// ================================================================================================
void LbdSimulation::additionalForcesCallback(reb_simulation *sim)
{
	m_pluginManager->additionalForces(sim);
}

// ================================================================================================
void LbdSimulation::preTimestepCallback(reb_simulation *sim)
{
	m_pluginManager->preTimestep(sim);
}

// ================================================================================================
void LbdSimulation::postTimestepCallback(reb_simulation *sim)
{
	m_pluginManager->postTimestep(sim);
}

// ================================================================================================
void LbdSimulation::heartbeatCallback(reb_simulation *sim)
{
	(++m_timestepCount);

	m_pluginManager->heartbeat(sim);

	if (!m_oManager->update()) {
		lerr("An error was detected with the update sequence.");
		// TODO: Allow the user to mark these as fatal if required
		reb_exit("An error occured in the heartbeat function, related to the output files.");
	}
}

// ================================================================================================
int LbdSimulation::collisionCallback(reb_simulation *sim, reb_collision col)
{
	int rem = m_pluginManager->collision(sim, col);
	if (rem == 1 || rem == 3) {
		m_pManager->removeParticleName(col.p1);
	}
	if (rem == 2 || rem == 3) {
		m_pManager->removeParticleName(col.p2);
	}

	return rem;
}

// ================================================================================================
void LbdSimulation::forceExit()
{
	reb_exit("Simulation encountered a fatal error, and could not complete.");
	exit(-1);
}

namespace luainterop
{

// ================================================================================================
void RegisterSimulationGlobals(sol::state& lua)
{
	// Register the function table for interacting with the active simulation
	lua["sim"] = lua.create_table_with(
		"addParticle", sol::overload(
			[](sol::object mass, sol::object radius, sol::object place, sol::object refpart, 
					sol::object name, sol::this_state state) -> sol::object {
				sol::state_view lua(state);
				LbdSimulation *sim = LbdSimulation::GetInstance();
				ParticleManager *pm = sim->getManager();
				ParticleFactory *pf = sim->getFactory();

				if (name.get_type() != sol::type::string) {
					lerr("The name specified for a particle must be a string.");
					throw "Logic Error";
				}
				String pname = name.as<String>();
				if (!_validateParticleName(pname)) {
					lerr(strfmt("The name '%s' is an invalid name for a single particle.", pname.c_str()));
					throw "Logic Error";
				}

				reb_particle part = pf->createParticle(mass, radius, place, refpart, pname, false);
				if (part.m < 0.0) {
					throw "Logic Error"; // The createParticle() function should report the error, only exit here
				}

				reb_particle *npart = pm->addParticle(pname, part);
				return sol::make_object(lua, sim_particle_ref(npart, false));
			}
		),
		"addParticles", sol::overload(
			[](sol::object count, sol::object mass, sol::object radius, sol::object place,
					sol::object refpart, sol::object name, sol::this_state state) -> void {
				sol::state_view lua(state); 
				LbdSimulation *sim = LbdSimulation::GetInstance();
				ParticleManager *pm = sim->getManager();
				ParticleFactory *pf = sim->getFactory();

				if (name.get_type() != sol::type::string) {
					lerr("The name specified for a particle must be a string.");
					throw "Logic Error";
				}
				String pname = name.as<String>();
				if (!_validateParticleName(pname)) {
					lerr(strfmt("The name '%s' is an invalid name for multiple particles.", pname.c_str()));
					throw "Logic Error";
				}

				if (count.get_type() != sol::type::number) {
					lerr("The amount of particles specified must be an integer number.");
					throw "Logic Error";
				}
				int pcount = count.as<int>();
				if (pcount <= 0) {
					lerr("The amount of particles must be a positive, finite integer.");
					throw "Logic Error";
				}

				for (int i = 0; i < pcount; ++i) {
					String thisname = pname;
					reb_particle part = pf->createParticle(mass, radius, place, refpart, thisname, true);
					if (part.m < 0.0) {
						throw "Logic Error"; // The createParticle() function should report the error, only exit here
					}

					reb_particle *npart = pm->addParticle(thisname, part);
				}
			}
		),
		"getParticle", sol::overload(
			[](sol::object ident, sol::this_state state) -> sol::object {
				sol::state_view lua(state);
				LbdSimulation *sim = LbdSimulation::GetInstance();
				ParticleManager *pm = sim->getManager();
				ParticleFactory *pf = sim->getFactory();
				
				reb_particle *part = nullptr;
				if (ident.get_type() == sol::type::number)
					part = pm->getParticleByHash(static_cast<uint32>(ident.as<double>()));
				else if (ident.get_type() == sol::type::string)
					part = pm->getParticleByName(ident.as<String>());
				else
					lwarn("The sim.getParticle() function must take a string or integer as an argument.");

				if (part)
					return sol::make_object(lua, sim_particle_ref(part, false));
				else
					return sol::nil;
			}
		),
		"setPrimaryParticle", sol::overload(
			[](sol::object primary) -> void {
				ParticleManager *pm = LbdSimulation::GetInstance()->getManager();

				if (primary == sol::nil) {
					pm->setPrimaryParticle(nullptr);
				}
				else if (primary.get_type() == sol::type::string) {
					String pname = primary.as<String>();
					if (!pm->setPrimaryParticle(pname)) {
						lerr(strfmt("Could not set the particle with name \"%s\" to be the primary particle.", 
								pname.c_str()));
						throw "Could not set primary particle.";
					}
				}
				else if (primary.get_type() == sol::type::number) {
					int32 hash = primary.as<int32>();
					if (!pm->setPrimaryParticle(hash)) {
						lerr(strfmt("Could not set the particle with hash %d to be the primary particle.", hash));
						throw "Could not set primary particle.";
					}
				}
				else if (primary.is<sim_particle_ref>()) {
					sim_particle_ref pref = primary.as<sim_particle_ref>();
					if (!pm->setPrimaryParticle(pref.get())) {
						lerr("Could not set the particle reference to be the primary particle.");
						throw "Could not set primary particle.";
					}
				}
				else {
					lerr("The primary particle provided must either be nil, a name, a hash, or a particle reference.");
					throw "Could not set primary particle.";
				}
			}
		)
	);

	// Register the main new_simulation function
	lua["new_simulation"] = [](sol::object obj) -> void {
		LbdSimulation *sim = LbdSimulation::GetInstance();

		if (!obj.is<sol::table>()) {
			lfatal("The argument provided to new_simulation must be a table.");
			sim->flagParseError();
			return;
		}

		sol::table simTable = obj.as<sol::table>();
		if (!sim->parseSimulationResults(simTable)) {
			sim->flagParseError();
			return;
		}
		if (!sim->populateSimulation(simTable)) {
			sim->flagPopulateError();
		}
	};
}

} // namespace luainterop


namespace callbacks
{

void additionalforces_callback(reb_simulation *sim)
{
	LbdSimulation::GetInstance()->additionalForcesCallback(sim);
}

void pretimestep_callback(reb_simulation *sim)
{
	LbdSimulation::GetInstance()->preTimestepCallback(sim);
}

void posttimestep_callback(reb_simulation *sim)
{
	LbdSimulation::GetInstance()->postTimestepCallback(sim);
}

void heartbeat_callback(reb_simulation *sim)
{
	LbdSimulation::GetInstance()->heartbeatCallback(sim);
}

int collision_callback(reb_simulation *sim, reb_collision col)
{
	return LbdSimulation::GetInstance()->collisionCallback(sim, col);
}

} // namespace callbacks