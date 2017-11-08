/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file includes and defines code that allows users to compile plugins that interact with
 *     luabound, programmed in C++, outside of the lua interface.
 */

#ifndef LUABOUND_USERHEADER_PLUGIN_HPP_
#define LUABOUND_USERHEADER_PLUGIN_HPP_

#include <Rebound.hpp>
#include <cstdint>
#include <string>

// The pointer to the internal luabound plugin structure 
//     (NOTE: USERS SHOULD NEVER TRY TO CHANGE THE VALUE OF THIS POINTER)
extern "C" void *__plugin_structure_ptr;

// The function pointers that are manipulated on the backend, and link to functions inside of luabound
extern "C" void(*__info_log_func_ptr)(const std::string& msg);
extern "C" void(*__warn_log_func_ptr)(const std::string& msg);
extern "C" void(*__error_log_func_ptr)(const std::string& msg);
extern "C" std::string(*__strfmt_func_ptr)(const std::string& fmt, ...);
extern "C" void(*__fatal_exit_func_ptr)(const std::string& msg);

// Expose the logging functions (the plugin should use these)
#define LogInfo(msg) do { (*__info_log_func_ptr)(msg); } while (false)
#define LogWarn(msg) do { (*__warn_log_func_ptr)(msg); } while (false)
#define LogError(msg) do { (*__error_log_func_ptr)(msg); } while (false)
#define Strfmt(fmt, ...) ((*__strfmt_func_ptr)(fmt, __VA_ARGS__))
#define FatalExit(msg) do { (*__fatal_exit_func_ptr)(msg); } while (false)

// The functions pointers to register callbacks, manipulated on the backend
extern "C" void(*__startup_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__shutdown_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__additionalforces_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__pretimestep_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__posttimestep_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__heartbeat_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim));
extern "C" void(*__collision_callback_register_func_ptr)(void * const plugin, int(*callback)(reb_simulation *sim, reb_collision col));

// Expose the callback registration functions
#define LbdRegisterStartupCallback(cback) \
	do { (*__startup_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterShutdownCallback(cback) \
	do { (*__shutdown_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterAdditionalForcesCallback(cback) \
	do { (*__additionalforces_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterPreTimestepCallback(cback) \
	do { (*__pretimestep_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterPostTimestepCallback(cback) \
	do { (*__posttimestep_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterHeartbeatCallback(cback) \
	do { (*__heartbeat_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)
#define LbdRegisterCollisionCallback(cback) \
	do { (*__collision_callback_register_func_ptr)(__plugin_structure_ptr, cback); } while (false)

// Provides global space for the symbols defined above, and is *required always*.
#define LBDPLUGIN_DEFINE_PLUGIN() \
	void *__plugin_structure_ptr = nullptr; \
	void(*__info_log_func_ptr)(const std::string& msg) = nullptr; \
	void(*__warn_log_func_ptr)(const std::string& msg) = nullptr; \
	void(*__error_log_func_ptr)(const std::string& msg) = nullptr; \
	std::string(*__strfmt_func_ptr)(const std::string& fmt, ...) = nullptr; \
	void(*__fatal_exit_func_ptr)(const std::string& msg) = nullptr; \
	void(*__startup_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__shutdown_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__additionalforces_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__pretimestep_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__posttimestep_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__heartbeat_callback_register_func_ptr)(void * const plugin, void(*callback)(reb_simulation *sim)) = nullptr; \
	void(*__collision_callback_register_func_ptr)(void * const plugin, int(*callback)(reb_simulation *sim, reb_collision col)) = nullptr;

#define LBDPLUGIN_INIT_FUNCTION() \
	extern "C" void plugin_initialize()

#endif // LUABOUND_USERHEADER_PLUGIN_HPP_