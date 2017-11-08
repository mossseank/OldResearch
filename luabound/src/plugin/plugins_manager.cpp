/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the PluginsManager class, which holds all of the simulation plugin
 *     instances, and manages updating them as necessary.
 */

#include "plugins_manager.hpp"

// ================================================================================================
PluginsManager::PluginsManager() :
	m_plugins{}
{

}

// ================================================================================================
PluginsManager::~PluginsManager()
{
	for (auto ptr : m_plugins)
		delete ptr;
	m_plugins.clear();
}

// ================================================================================================
void PluginsManager::startup(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->startup(sim);
}

// ================================================================================================
void PluginsManager::shutdown(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->shutdown(sim);
}

// ================================================================================================
void PluginsManager::additionalForces(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->additionalForces(sim);
}

// ================================================================================================
void PluginsManager::preTimestep(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->preTimestep(sim);
}

// ================================================================================================
void PluginsManager::postTimestep(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->postTimestep(sim);
}

// ================================================================================================
void PluginsManager::heartbeat(reb_simulation *sim)
{
	for (auto ptr : m_plugins)
		ptr->heartbeat(sim);
}

// ================================================================================================
int PluginsManager::collision(reb_simulation *sim, reb_collision col)
{
	int ret = 0;
	for (auto ptr : m_plugins)
		ret = ptr->collision(sim, col);
	return ret;
}

// ================================================================================================
bool PluginsManager::loadPlugins(sol::table& plugins)
{
	StlVector<String> names;
	bool result = true;
	plugins.for_each([&result, &names](sol::object value, sol::object key) -> void {
		if (!result)
			return; // Cut the parsing early if there is already an error

		if (key.get_type() != sol::type::string) {
			lerr("Value specified for a plugin was not a string.");
			result = false;
			return;
		}

		names.push_back(key.as<String>());
	});

	if (!result)
		return false;

	for (const auto& str : names)
		m_plugins.push_back(new Plugin(str));

	for (auto ptr : m_plugins) {
		if (!ptr->load()) {
			lfatal(strfmt("The plugin '%s' could not be loaded.", ptr->getName().c_str()));
			return false;
		}
	}

	return true;
}