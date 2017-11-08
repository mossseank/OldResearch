/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements all of the values and objects used to parse the format strings in the simulation
 *     output table.
 */

#include "format_parser.hpp"
#include "../simulation.hpp"
#include <regex>

// Regex strings for finding patterns
static const String PUNCTUATION_TOKEN_REGEX_STR = R"([,;:\/\\ \t]+)";
static const String VALUE_TOKEN_REGEX_STR = R"(#(\w)(\w\w?))";
static const String LIST_SPECIFIER_REGEX_STR = R"(\{(.*?)\})";
static const String FORMAT_REGEX_FULL_STR = 
		"(?:" + LIST_SPECIFIER_REGEX_STR + ")|(?:" + VALUE_TOKEN_REGEX_STR + ")|(?:" + PUNCTUATION_TOKEN_REGEX_STR + ")";


namespace
{

format_ast::base_node* _parseValueToken(std::smatch& match, bool list)
{
	String matchStr = match[0].str();
	String tag = match[2].str();
	String value = match[3].str();
	ValueGroup group = token_utils::StringToValueGroup(tag);

	if (group == ValueGroup::INVALID) {
		lerr(strfmt("The value token %s does not specify a valid group identifier.", matchStr.c_str()));
		return nullptr;
	}
	else if (group == ValueGroup::Simulation) {
		ValueSType stype = token_utils::StringToValueSType(value);
		if (stype == ValueSType::INVALID) {
			lerr(strfmt("The value token %s does not specify a valid simulation value.", matchStr.c_str()));
			return nullptr;
		}
		return new format_ast::svalue_token_node(stype);
	}
	else if (group == ValueGroup::Average || group == ValueGroup::StdDev) {
		ValuePType ptype = token_utils::StringToValuePType(value);
		if (ptype == ValuePType::INVALID) {
			lerr(strfmt("The value token %s does not specify a valid particle value.", matchStr.c_str()));
			return nullptr;
		}
		if (ptype == ValuePType::Name) {
			lerr("Cannot request global particle names.");
			return nullptr;
		}
		if (ptype == ValuePType::Hash) {
			lerr("Cannot request global particle hashes.");
			return nullptr;
		}
		return new format_ast::pvalue_token_node(group, ptype);
	}
	else if (group == ValueGroup::Particle) {
		if (!list) {
			lerr(strfmt("The particle value token %s can only be used inside of list specifiers.", matchStr.c_str()));
			return nullptr;
		}
		ValuePType ptype = token_utils::StringToValuePType(value);
		if (ptype == ValuePType::INVALID) {
			lerr(strfmt("The value token %s does not specify a valid particle value.", matchStr.c_str()));
			return nullptr;
		}
		return new format_ast::pvalue_token_node(group, ptype);
	}

	return nullptr;
}

format_ast::base_node* _parseListSpecifier(const String& liststr, bool lastNode)
{
	static const std::regex FULL_REGEX(FORMAT_REGEX_FULL_STR, 
			std::regex_constants::ECMAScript | std::regex_constants::optimize);
	
	std::smatch match;
	size_t currentStart = 0;
	String currentListStr = liststr;
	format_ast::list_node::node_ptr_list nodeList;
	while (std::regex_search(currentListStr, match, FULL_REGEX, 
			std::regex_constants::match_continuous | std::regex_constants::match_not_null)) {
		currentStart += match.length();

		format_ast::base_node *node = nullptr;
		String matchStr = match.str();
		if (matchStr[0] == '#') { // Value token
			node = _parseValueToken(match, true);
			if (!node)
				return nullptr;
		}
		else if (matchStr[0] == '{') { // List Specifier
			lerr(strfmt("Cannot embed a list specifier inside another list specifier (\"%s\").", 
					liststr.c_str()));
			return nullptr;
		}
		else { // Punctuation 
			String puncStr = match[0].str();
			node = new format_ast::punctuation_node(puncStr);
		}

		nodeList.push_back(node);
		currentListStr = liststr.substr(currentStart);
	}

	if (currentListStr.length()) {
		lerr(strfmt("Could not completely parse list specifier string, failed on \"%s\".", 
					currentListStr.c_str()));
		return nullptr;
	}

	return new format_ast::list_node(nodeList, lastNode);
}

} // namespace


// ================================================================================================
bool OutputFormat::loadFormat(const String& fmt)
{
	static const std::regex FULL_REGEX(FORMAT_REGEX_FULL_STR, 
			std::regex_constants::ECMAScript | std::regex_constants::optimize);

	std::smatch match;
	size_t currentStart = 0;
	String currentFmtString = fmt;
	while (std::regex_search(currentFmtString, match, FULL_REGEX, 
			std::regex_constants::match_continuous | std::regex_constants::match_not_null)) {
		currentStart += match.length();

		FormatNode *node = nullptr;
		String matchStr = match.str();
		if (matchStr[0] == '#') { // Value token
			node = _parseValueToken(match, false);
			if (!node)
				return false;
		}
		else if (matchStr[0] == '{') { // List Specifier
			node = _parseListSpecifier(match[1].str(), fmt.substr(currentStart).length() == 0);
			if (!node)
				return false;
		}
		else { // Punctuation 
			String puncStr = match[0].str();
			node = new format_ast::punctuation_node(puncStr);
		}

		m_formats.emplace_back(std::move(StlUniquePtr<FormatNode>(node)));
		currentFmtString = fmt.substr(currentStart);
	}

	if (currentFmtString.length()) {
		lerr(strfmt("Could not completely parse format string, failed on \"%s\".", currentFmtString.c_str()));
		return false;
	}

	return true;
}

// ================================================================================================
void OutputFormat::generateOutput(LbdSimulation *sim, StringStream& out)
{
	for (auto& nodeptr : m_formats) {
		nodeptr->generateOutput(sim, out);
	}
}