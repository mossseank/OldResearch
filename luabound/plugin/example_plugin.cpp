/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file provides an example luabound plugin file structure.
 */

// Every plugin must include this file
#include "luabound_plugin.hpp"

// Required to be at the top of every plugin file. The plugin *will not work* without this.
LBDPLUGIN_DEFINE_PLUGIN();

// Declare the functions that we will register as callbacks (these can be any name)
void plugin_startup(reb_simulation *sim);
void plugin_shutdown(reb_simulation *sim);
void plugin_additionalForces(reb_simulation *sim);
void plugin_preTimestep(reb_simulation *sim);
void plugin_postTimestep(reb_simulation *sim);
void plugin_heartbeat(reb_simulation *sim);
int plugin_collision(reb_simulation *sim, reb_collision col);

// This defines the entry point for the plugin. This is where you should register callbacks.
//     Further resource initialization should be done in the startup callback, not here.
LBDPLUGIN_INIT_FUNCTION()
{
	LogInfo("Info logging from plugin.");
	LogWarn("Warning logging from plugin.");
	LogError(Strfmt("Formatted %s logging from plugin.", "error"));

	LbdRegisterStartupCallback(plugin_startup);
	LbdRegisterShutdownCallback(plugin_shutdown);
	LbdRegisterAdditionalForcesCallback(plugin_additionalForces);
	LbdRegisterPreTimestepCallback(plugin_preTimestep);
	LbdRegisterPostTimestepCallback(plugin_postTimestep);
	LbdRegisterHeartbeatCallback(plugin_heartbeat);
	LbdRegisterCollisionCallback(plugin_collision);
}

// Called after the simulation is populated, should be used to check initial conditions and
//     load/create any plugin-specific resources.
void plugin_startup(reb_simulation *sim)
{
	LogInfo("Plugin startup");
}

// Called when the simulation is shutting down. Unload/destroy any plugin-specific resources here.
void plugin_shutdown(reb_simulation *sim)
{
	LogInfo("Plugin shutdown");
}

// Called to add additional forces to the simulation
void plugin_additionalForces(reb_simulation *sim)
{
	static int count = 0;
	if (count++ < 3)
		LogInfo("Plugin additionalForces");
}

// Called before the simulation timestep occurs
void plugin_preTimestep(reb_simulation *sim)
{
	static int count = 0;
	if (count++ < 3)
		LogInfo("Plugin preTimestep");
}

// Called after the simulation timestep occurs
void plugin_postTimestep(reb_simulation *sim)
{
	static int count = 0;
	if (count++ < 3)
		LogInfo("Plugin postTimestep");
}

// Called during each timestep
void plugin_heartbeat(reb_simulation *sim)
{
	static int count = 0;
	if (count++ < 3)
		LogInfo("Plugin heartbeat");
}

// Called when a collision happens in the simulation
int plugin_collision(reb_simulation *sim, reb_collision col)
{
	LogInfo("Plugin collision");
	return 0;
}