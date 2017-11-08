/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#pragma once
#define FORMAT_PARSER_HPP_

#include "format_token.hpp"


class OutputFormat
{
private:
	using FormatNode = format_ast::base_node;
	using ASTList = std::vector<std::unique_ptr<FormatNode>>;

	ASTList m_formats;

public:
	OutputFormat() { }
	~OutputFormat() {
		m_formats.clear();
	}

	bool loadFormat(const std::string& fmt);

	void generateOutput(reb_simulation *sim, std::stringstream& out);
};