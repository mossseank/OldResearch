/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file defines the Plugin class, which holds a reference to a loaded plugin, as well as
 *     symbols within the plugin.
 */

#ifndef PLUGIN_HPP_
#define PLUGIN_HPP_

#include "../luabound.hpp"

using LogFcnType = void(*)(const String&);
using StrFmtFcnType = String(*)(const String&, ...);
using PluginInitFcnType = void(*)();
using CallbackRegisterFcnType = void(*)(void * const, void(*)(reb_simulation*));
using CollisionCallbackRegisterFcnType = void(*)(void * const, int(*)(reb_simulation*, reb_collision));
using CallbackFcnType = void(*)(reb_simulation*);
using CollisionCallbackFcnType = int(*)(reb_simulation*, reb_collision);

class Plugin
{
private:
	const String m_name;
	const String m_fileName;
	void *m_libHandle;
	struct
	{
		LogFcnType *info;
		LogFcnType *warn;
		LogFcnType *error;
		StrFmtFcnType *strfmt;
	} m_logFcnHandles;
	struct
	{
		LogFcnType *fatalExit;
	} m_controlFcnHandles;
	struct
	{
		CallbackRegisterFcnType *startup;
		CallbackRegisterFcnType *shutdown;
		CallbackRegisterFcnType *additionalForces;
		CallbackRegisterFcnType *preTimestep;
		CallbackRegisterFcnType *postTimestep;
		CallbackRegisterFcnType *heartbeat;
		CollisionCallbackRegisterFcnType *collision;
	} m_callbackRegisterFcnHandles;
	struct
	{
		CallbackFcnType startup;
		CallbackFcnType shutdown;
		CallbackFcnType additionalForces;
		CallbackFcnType preTimestep;
		CallbackFcnType postTimestep;
		CallbackFcnType heartbeat;
		CollisionCallbackFcnType collision;
	} m_callbackFcnHandles;
	PluginInitFcnType m_initFcnHandle;

public:
	Plugin(const String& name);
	~Plugin();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(Plugin)

	inline String getName() const { return m_name; }
	inline bool isLoaded() const { return (m_libHandle != nullptr); }

	inline void startup(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.startup) { m_callbackFcnHandles.startup(sim); } }
	inline void shutdown(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.shutdown) { m_callbackFcnHandles.shutdown(sim); } }
	inline void additionalForces(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.additionalForces) { m_callbackFcnHandles.additionalForces(sim); } }
	inline void preTimestep(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.preTimestep) { m_callbackFcnHandles.preTimestep(sim); } }
	inline void postTimestep(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.postTimestep) { m_callbackFcnHandles.postTimestep(sim); } }
	inline void heartbeat(reb_simulation *sim) 
			{ if (m_callbackFcnHandles.heartbeat) { m_callbackFcnHandles.heartbeat(sim); } }
	inline int collision(reb_simulation *sim, reb_collision col)
			{ if (m_callbackFcnHandles.collision) { return m_callbackFcnHandles.collision(sim, col); } }

	bool load();

private:
	bool loadPluginSymbols();
	bool populatePluginSymbols();
};

#endif // PLUGIN_HPP_