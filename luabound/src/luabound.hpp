/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file includes and defines parts of code that are common to most of the application. Simply
 *     including this file should take care of all of the header files that are generally needed.
 *     This includes the header files for Rebound and Lua. It also defines the functions for
 *     logging formatted messages from the library.
 */

#ifndef LUABOUND_LUABOUND_HPP_
#define LUABOUND_LUABOUND_HPP_

// Include the Rebound API
#include <Rebound.hpp>

// Include the Lua API and wrapper through sol2
#define SOL_CHECK_ARGUMENTS
#include <sol.hpp>

// Include some standard stl headers
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>

// Include some standard stl containers
#include <array>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

// Expose the C++ string
using String = std::string;
using StringStream = std::stringstream;

// Expose the standard size stl types
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

// Expose the stl container classes
template <
	class T,
	std::size_t N
> using StlArray = std::array<T, N>;
template <
	class T,
	class Allocator = std::allocator<T>
> using StlDeque = std::deque<T, Allocator>;
template <
	class T,
	class Allocator = std::allocator<T>
> using StlList = std::list<T, Allocator>;
template <
	class T,
	class Container = std::deque<T>
> using StlQueue = std::queue<T, Container>;
template <
	class T,
	class Container = std::deque<T>
> using StlStack = std::stack<T, Container>;
template <
	class Key,
	class T,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>,
	class Allocator = std::allocator<std::pair<const Key, T>>
> using StlHashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
template <
	class T,
	class Allocator = std::allocator<T>
> using StlVector = std::vector<T, Allocator>;
template <
	class T
> using StlInitializerList = std::initializer_list<T>;

// Expose the stl smart pointer classes
template <
	class T,
	class Deleter = std::default_delete<T>
> using StlUniquePtr = std::unique_ptr<T, Deleter>;
template <
	class T
> using StlSharedPtr = std::shared_ptr<T>;
template <
	class T
> using StlWeakPtr = std::weak_ptr<T>;

// Macro definition for declaring classes as non-copyable and non-moveable
#define LUABOUND_DECLARE_CLASS_NONCOPYABLE(className) \
	public: \
		className (const className&) = delete; \
		className& operator = (const className&) = delete;

// Make sure we have the all important M_PI (use Rebound's M_PI if not)
#ifndef M_PI
#	define M_PI (3.14159265358979323846)
#endif // M_PI

// Define the internal logging functionality
String strfmt(const String& fmt, ...);

void linfo(const String& msg);
void lsim(const String& msg); // This should only ever be used by luabound internally for stdout output
void lplugin(const String& msg); // This should only ever be used for plugin output
void lwarn(const String& msg);
void lerr(const String& msg);
void lfatal(const String& msg);
void lsetPrefix(const String& pre);

// Stringification macros
#define xstrify(a) __strify(a)
#define __strify(a) #a

// Define the versioning information
#define LUABOUND_VERSION_MAJOR 0
#define LUABOUND_VERSION_MINOR 1
#define LUABOUND_VERSION_REVISION 0
#define LUABOUND_VERSION (xstrify(LUABOUND_VERSION_MAJOR) "." xstrify(LUABOUND_VERSION_MINOR) "." xstrify(LUABOUND_VERSION_REVISION))

// Platform detection
#ifdef _WIN32
#	error You cannot compile luabound on Windows (even with Mingw or Cygwin)
#elif __APPLE__
#	include "TargetConditionals.h"
#	if TARGET_OS_MAC
#		define LUABOUND_PLATFORM_MACOS 
#	else
#		error You can only compile luabound on the desktop version of MacOS
#	endif // TARGET_OS_MAC
#elif __linux__
#	define LUABOUND_PLATFORM_LINUX 
#else
#	error Unknown platform, cannot compile luabound
#endif // _WIN32

#endif // LUABOUND_LUABOUND_HPP_