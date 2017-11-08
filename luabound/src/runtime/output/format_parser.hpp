/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares all of the values and objects used to parse the format strings in the simulation
 *     output table.
 */

#ifndef FORMAT_PARSER_HPP_
#define FORMAT_PARSER_HPP_

#include "../../luabound.hpp"
#include "format_token.hpp"

class OutputFormat
{
private:
	using FormatNode = format_ast::base_node;
	using ASTList = StlVector<StlUniquePtr<FormatNode>>;

	ASTList m_formats;

public:
	OutputFormat() { }
	~OutputFormat() {
		m_formats.clear();
	}

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(OutputFormat)

	bool loadFormat(const String& fmt);

	void generateOutput(LbdSimulation *sim, StringStream& out);
};

#endif // FORMAT_PARSER_HPP_