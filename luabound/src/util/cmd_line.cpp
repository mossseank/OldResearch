/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements structures and functions related to parsing command line arguments.
 */

#include "cmd_line.hpp"
#include <regex>

enum _RegexMatchType
{
	MATCH_NONE = 0,
	MATCH_FLAG = 1,
	MATCH_OPTION = 2
};

// Uses regex to attempt to match the command line argument
_RegexMatchType _extractParameter(const char* param, String& name, String& value)
{
	// The hyphens and flag name are stored in group 1
	static const std::regex FLAG_PATTERN(R"(^(-[\w\d]|--[\w\d]+)$)", 
		std::regex_constants::ECMAScript | std::regex_constants::optimize);
	// The hyphens and flag name are stored in group 1, the value is stored in group 2
	static const std::regex OPTION_PATTERN(R"(^(-[\w\d]|--[\w\d]+)="?(.*?)\"?$)", 
		std::regex_constants::ECMAScript | std::regex_constants::optimize);

	std::smatch match;
	String paramstr(param);
	if (std::regex_match(paramstr, match, FLAG_PATTERN))
	{
		name = match[1].str();
		if (name[1] == '-')
			name = name.substr(2);
		else
			name = name.substr(1);

		return MATCH_FLAG;
	}
	
	if (std::regex_match(paramstr, match, OPTION_PATTERN))
	{
		name = match[1].str();
		if (name[1] == '-')
			name = name.substr(2);
		else
			name = name.substr(1);
		
		value = match[2].str();

		return MATCH_OPTION;
	}

	return MATCH_NONE;
}

// ================================================================================================
void parse_command_line(int argc, char **argv, cmd_line_parameters& params)
{
	int totalLoaded = 0;
	for (int i = 1; i < argc; ++i)
	{
		String name;
		String value;
		_RegexMatchType match = _extractParameter(argv[i], name, value);

		if (match == MATCH_NONE)
		{
			lwarn(strfmt("Ignoring command line parameter '%s' for being ill-formatted.", argv[i]));
			continue;
		}

		if (match == MATCH_OPTION && name == "file")
		{
			static bool fileSet = false;

			if (fileSet)
			{
				lwarn(strfmt("Overwriting previous --file setting of '%s' with new value '%s'.", 
					params.scriptFile.c_str(), value.c_str()));
			}

			params.scriptFile = value;
			fileSet = true;
		}
		else
		{
			lwarn(strfmt("Ignoring command line parameter '%s' for not being recognized.", argv[i]));
			continue;
		}

		totalLoaded++;
	}

	linfo(strfmt("Loaded %d command line parameters.", totalLoaded));
}