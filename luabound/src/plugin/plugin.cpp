/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the Plugin class, which holds a reference to a loaded plugin, as well as
 *     symbols within the plugin.
 */

#include "plugin.hpp"
#include "../runtime/simulation.hpp"
#include <dlfcn.h>

// ================================================================================================
Plugin::Plugin(const String& name) :
	m_name{name},
	m_fileName{"lib" + name + ".so"},
	m_libHandle{nullptr},
	m_logFcnHandles{nullptr, nullptr, nullptr, nullptr},
	m_controlFcnHandles{nullptr},
	m_callbackRegisterFcnHandles{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
	m_callbackFcnHandles{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
	m_initFcnHandle{nullptr}
{

}

// ================================================================================================
Plugin::~Plugin()
{
	if (m_libHandle && dlclose(m_libHandle)) {
		lerr(strfmt("Could not unload library file '%s'. Reason: '%s'.", m_fileName.c_str(), dlerror()));
	}
}

// ================================================================================================
bool Plugin::load()
{
	m_libHandle = dlopen(("./" + m_fileName).c_str(), RTLD_LAZY | RTLD_LOCAL);
	if (!m_libHandle) {
		lerr(strfmt("Could not load the library file '%s'. Reason: '%s'.", m_fileName.c_str(), dlerror()));
		return false;
	}

	if (!loadPluginSymbols())
		return false;

	if (!populatePluginSymbols())
		return false;

	(m_initFcnHandle)(); // Call plugin initialization function

	return true;
}

// ================================================================================================
bool Plugin::loadPluginSymbols()
{
	void *pluginPtr = dlsym(m_libHandle, "__plugin_structure_ptr");
	if (!pluginPtr) {
		lerr(strfmt("Could not load plugin structure pointer symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	} 
	(*static_cast<void**>(pluginPtr)) = static_cast<void*>(this);

	m_logFcnHandles.info = static_cast<LogFcnType*>(dlsym(m_libHandle, "__info_log_func_ptr"));
	if (!m_logFcnHandles.info) {
		lerr(strfmt("Could not load logging info function symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_logFcnHandles.warn = static_cast<LogFcnType*>(dlsym(m_libHandle, "__warn_log_func_ptr"));
	if (!m_logFcnHandles.warn) {
		lerr(strfmt("Could not load logging warn function symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_logFcnHandles.error = static_cast<LogFcnType*>(dlsym(m_libHandle, "__error_log_func_ptr"));
	if (!m_logFcnHandles.error) {
		lerr(strfmt("Could not load logging error function symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_logFcnHandles.strfmt = static_cast<StrFmtFcnType*>(dlsym(m_libHandle, "__strfmt_func_ptr"));
	if (!m_logFcnHandles.strfmt) {
		lerr(strfmt("Could not load string format function symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_controlFcnHandles.fatalExit = static_cast<LogFcnType*>(dlsym(m_libHandle, "__fatal_exit_func_ptr"));
	if (!m_controlFcnHandles.fatalExit) {
		lerr(strfmt("Could not load fatal exit function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_initFcnHandle = reinterpret_cast<PluginInitFcnType>(dlsym(m_libHandle, "plugin_initialize"));
	if (!m_initFcnHandle) {
		lerr(strfmt("Could not load plugin initialization function for plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.startup = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__startup_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.startup) {
		lerr(strfmt("Could not load startup callback register function symbol from plugin '%s'. Reason: '%s'.", 
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.shutdown = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__shutdown_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.shutdown) {
		lerr(strfmt("Could not load shutdown callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.additionalForces = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__additionalforces_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.additionalForces) {
		lerr(strfmt("Could not load additional forces callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.preTimestep = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__pretimestep_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.preTimestep) {
		lerr(strfmt("Could not load preTimestep callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.postTimestep = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__posttimestep_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.postTimestep) {
		lerr(strfmt("Could not load postTimestep callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.heartbeat = 
			static_cast<CallbackRegisterFcnType*>(dlsym(m_libHandle, "__heartbeat_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.heartbeat) {
		lerr(strfmt("Could not load heartbeat callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	m_callbackRegisterFcnHandles.collision = 
			static_cast<CollisionCallbackRegisterFcnType*>(dlsym(m_libHandle, "__collision_callback_register_func_ptr"));
	if (!m_callbackRegisterFcnHandles.collision) {
		lerr(strfmt("Could not load collision callback register function symbol from plugin '%s'. Reason: '%s'.",
				m_name.c_str(), dlerror()));
		return false;
	}

	return true;
}

// ================================================================================================
bool Plugin::populatePluginSymbols()
{
	*(m_logFcnHandles.info) = [](const String& str) -> void {
		String msg = "Info:  " + str;
		lplugin(msg);
	};
	*(m_logFcnHandles.warn) = [](const String& str) -> void {
		String msg = "Warn:  " + str;
		lplugin(msg);
	};
	*(m_logFcnHandles.error) = [](const String& str) -> void {
		String msg = "Error: " + str;
		lplugin(msg);
	};
	*(m_logFcnHandles.strfmt) = strfmt;

	*(m_controlFcnHandles.fatalExit) = [](const String& str) -> void {
		lfatal(strfmt("\nPlugin encountered a fatal error! Message: '%s'.\n", str.c_str()));
		LbdSimulation::GetInstance()->forceExit();
	};

	*(m_callbackRegisterFcnHandles.startup) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.startup != nullptr)
			lwarn(strfmt("Overwriting previous startup callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.startup = callback;
	};
	*(m_callbackRegisterFcnHandles.shutdown) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.shutdown != nullptr)
			lwarn(strfmt("Overwriting previous shutdown callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.shutdown = callback;
	};
	*(m_callbackRegisterFcnHandles.additionalForces) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.additionalForces != nullptr)
			lwarn(strfmt("Overwriting previous additionalForces callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.additionalForces = callback;
	};
	*(m_callbackRegisterFcnHandles.preTimestep) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.preTimestep != nullptr)
			lwarn(strfmt("Overwriting previous preTimestep callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.preTimestep = callback;
	};
	*(m_callbackRegisterFcnHandles.postTimestep) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.postTimestep != nullptr)
			lwarn(strfmt("Overwriting previous postTimestep callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.postTimestep = callback;
	};
	*(m_callbackRegisterFcnHandles.heartbeat) = [](void * const plugin, CallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.heartbeat != nullptr)
			lwarn(strfmt("Overwriting previous heartbeat callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.heartbeat = callback;
	};
	*(m_callbackRegisterFcnHandles.collision) = [](void * const plugin, CollisionCallbackFcnType callback) -> void {
		Plugin * const plg = static_cast<Plugin * const>(plugin);
		if (plg->m_callbackFcnHandles.collision != nullptr)
			lwarn(strfmt("Overwriting previous collision callback in plugin '%s'.", plg->m_name.c_str()));
		plg->m_callbackFcnHandles.collision = callback;
	};

	return true;
}