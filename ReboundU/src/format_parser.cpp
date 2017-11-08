/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <format_parser.hpp>
#include <regex>
#include <rebu_error.hpp>


// Regex strings for finding patterns
static const std::string PUNCTUATION_TOKEN_REGEX_STR = R"([,;:\/\\ \t]+)";
static const std::string VALUE_TOKEN_REGEX_STR = R"(#(\w)(\w\w?))";
static const std::string LIST_SPECIFIER_REGEX_STR = R"(\{(.*?)\})";
static const std::string FORMAT_REGEX_FULL_STR = 
		"(?:" + LIST_SPECIFIER_REGEX_STR + ")|(?:" + VALUE_TOKEN_REGEX_STR + ")|(?:" + PUNCTUATION_TOKEN_REGEX_STR + ")";


namespace
{

format_ast::base_node* _parseValueToken(std::smatch& match, bool list)
{
	std::string matchStr = match[0].str();
	std::string tag = match[2].str();
	std::string value = match[3].str();
	ValueGroup group = token_utils::StringToValueGroup(tag);

	if (group == ValueGroup::INVALID) {
		_rebu_set_last_error_message("The value token %s does not specify a valid group identifier.", matchStr.c_str());
		return nullptr;
	}
	else if (group == ValueGroup::Simulation) {
		ValueSType stype = token_utils::StringToValueSType(value);
		if (stype == ValueSType::INVALID) {
			_rebu_set_last_error_message("The value token %s does not specify a valid simulation value.", matchStr.c_str());
			return nullptr;
		}
		return new format_ast::svalue_token_node(stype);
	}
	else if (group == ValueGroup::Average || group == ValueGroup::StdDev) {
		ValuePType ptype = token_utils::StringToValuePType(value);
		if (ptype == ValuePType::INVALID) {
			_rebu_set_last_error_message("The value token %s does not specify a valid particle value.", matchStr.c_str());
			return nullptr;
		}
		if (ptype == ValuePType::Name) {
			_rebu_set_last_error_message("Cannot request global particle names.");
			return nullptr;
		}
		if (ptype == ValuePType::Hash) {
			_rebu_set_last_error_message("Cannot request global particle hashes.");
			return nullptr;
		}
		return new format_ast::pvalue_token_node(group, ptype);
	}
	else if (group == ValueGroup::Particle) {
		if (!list) {
			_rebu_set_last_error_message("The particle value token %s can only be used inside of list specifiers.", matchStr.c_str());
			return nullptr;
		}
		ValuePType ptype = token_utils::StringToValuePType(value);
		if (ptype == ValuePType::INVALID) {
			_rebu_set_last_error_message("The value token %s does not specify a valid particle value.", matchStr.c_str());
			return nullptr;
		}
		return new format_ast::pvalue_token_node(group, ptype);
	}

	return nullptr;
}

format_ast::base_node* _parseListSpecifier(const std::string& liststr, bool lastNode)
{
	static const std::regex FULL_REGEX(FORMAT_REGEX_FULL_STR, 
			std::regex_constants::ECMAScript | std::regex_constants::optimize);
	
	std::smatch match;
	size_t currentStart = 0;
	std::string currentListStr = liststr;
	format_ast::list_node::node_ptr_list nodeList;
	while (std::regex_search(currentListStr, match, FULL_REGEX, 
			std::regex_constants::match_continuous | std::regex_constants::match_not_null)) {
		currentStart += match.length();

		format_ast::base_node *node = nullptr;
		std::string matchStr = match.str();
		if (matchStr[0] == '#') { // Value token
			node = _parseValueToken(match, true);
			if (!node)
				return nullptr;
		}
		else if (matchStr[0] == '{') { // List Specifier
			_rebu_set_last_error_message("Cannot embed a list specifier inside another list specifier (\"%s\").", liststr.c_str());
			return nullptr;
		}
		else { // Punctuation 
			std::string puncStr = match[0].str();
			node = new format_ast::punctuation_node(puncStr);
		}

		nodeList.push_back(node);
		currentListStr = liststr.substr(currentStart);
	}

	if (currentListStr.length()) {
		_rebu_set_last_error_message("Could not completely parse list specifier string, failed on \"%s\".", currentListStr.c_str());
		return nullptr;
	}

	return new format_ast::list_node(nodeList, lastNode);
}

} // namespace


// ================================================================================================
bool OutputFormat::loadFormat(const std::string& fmt)
{
	static const std::regex FULL_REGEX(FORMAT_REGEX_FULL_STR, 
			std::regex_constants::ECMAScript | std::regex_constants::optimize);

	std::smatch match;
	size_t currentStart = 0;
	std::string currentFmtString = fmt;
	while (std::regex_search(currentFmtString, match, FULL_REGEX, 
			std::regex_constants::match_continuous | std::regex_constants::match_not_null)) {
		currentStart += match.length();

		FormatNode *node = nullptr;
		std::string matchStr = match.str();
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
			std::string puncStr = match[0].str();
			node = new format_ast::punctuation_node(puncStr);
		}

		m_formats.emplace_back(std::move(std::unique_ptr<FormatNode>(node)));
		currentFmtString = fmt.substr(currentStart);
	}

	if (currentFmtString.length()) {
		_rebu_set_last_error_message("Could not completely parse format string, failed on \"%s\".", currentFmtString.c_str());
		return false;
	}

	return true;
}

// ================================================================================================
void OutputFormat::generateOutput(reb_simulation *sim, std::stringstream& out)
{
	for (auto& nodeptr : m_formats) {
		nodeptr->generateOutput(sim, out);
	}
}