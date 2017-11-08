/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the logging functions that are found in the luabound.hpp header file.
 */

#include "../luabound.hpp"
#include <iomanip>
#include <ctime>

static const String INFO_TAG  = "INFO:(%H:%M:%S)> ";
static const String SIM_TAG   = "SIML:(%H:%M:%S)> ";
static const String PLGN_TAG  = "PLGN:(%H:%M:%S)> ";
static const String WARN_TAG  = "WARN:(%H:%M:%S)> ";
static const String ERR_TAG   = "ERRO:(%H:%M:%S)> ";
static const String FATAL_TAG = "FATL:(%H:%M:%S)> ";
static const size_t TAG_LEN   = INFO_TAG.length();
static String prefix = "";
static size_t prefix_len = 0;

void _formatTimeString(const String& fmt, String& out)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	StringStream ss;
	ss << std::put_time(&tm, fmt.c_str());
	out = ss.str();
}

void _formatNewlines(const String& in, String& out)
{
	size_t pos = 0;
	out = in;
	String newline_padding = String(TAG_LEN + prefix_len, ' ');
	while ((pos = out.find('\n', pos + 1)) != String::npos)
	{
		out.insert(pos + 1, newline_padding);
	}
}

// ================================================================================================
String strfmt(const String& fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

	char dst[1024]; // Evil fixed limit message length, but oh well for now
	vsnprintf(dst, 1023, fmt.c_str(), argptr);
	va_end(argptr);

	return dst;
}

// ================================================================================================
void linfo(const String& msg)
{
	String tag;
	_formatTimeString(INFO_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cout << out << std::endl;
}

// ================================================================================================
void lsim(const String& msg)
{
	String tag;
	_formatTimeString(SIM_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cout << out << std::endl;
}

// ================================================================================================
void lplugin(const String& msg)
{
	String tag;
	_formatTimeString(PLGN_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cout << out << std::endl;
}

// ================================================================================================
void lwarn(const String& msg)
{
	String tag;
	_formatTimeString(WARN_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cout << out << std::endl;
}

// ================================================================================================
void lerr(const String& msg)
{
	String tag;
	_formatTimeString(ERR_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cerr << out << std::endl;
}

// ================================================================================================
void lfatal(const String& msg)
{
	String tag;
	_formatTimeString(FATAL_TAG, tag);
	String out;
	_formatNewlines(tag + prefix + msg, out);

	std::cerr << out << std::endl;
}

// ================================================================================================
void lsetPrefix(const String& pre)
{
	prefix = pre;
	prefix_len = pre.length();
}