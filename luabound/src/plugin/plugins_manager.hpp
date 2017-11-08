/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file defines the PluginsManager class, which holds all of the simulation plugin
 *     instances, and manages updating them as necessary.
 */

#ifndef PLUGIN_MANAGER_HPP_
#define PLUGIN_MANAGER_HPP_

#include "../luabound.hpp"
#include "plugin.hpp"

class PluginsManager
{
private:
	StlVector<Plugin*> m_plugins;

public:
	PluginsManager();
	~PluginsManager();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(PluginsManager)

	inline int getPluginCount() const { return m_plugins.size(); }

	void startup(reb_simulation *sim);
	void shutdown(reb_simulation *sim);
	void additionalForces(reb_simulation *sim);
	void preTimestep(reb_simulation *sim);
	void postTimestep(reb_simulation *sim);
	void heartbeat(reb_simulation *sim);
	int collision(reb_simulation *sim, reb_collision col);

	bool loadPlugins(sol::table& plugins);
};

#endif // PLUGIN_MANAGER_HPP_