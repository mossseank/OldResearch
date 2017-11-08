/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the NameFactory class, which manages the progressive particle naming
 *     schemes.
 */

#ifndef LUABOUND_NAME_FACTORY_HPP_
#define LUABOUND_NAME_FACTORY_HPP_

#include "../../luabound.hpp"

class NameFactory
{
private:
	using NameMap = StlHashMap<String, uint32>;

	NameMap m_names;

public:
	NameFactory();
	~NameFactory();

	bool hasName(const String& name);
	void addName(const String& name);

	String getNext(const String& name);
};

#endif // LUABOUND_NAME_FACTORY_HPP_