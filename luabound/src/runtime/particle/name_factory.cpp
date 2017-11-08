/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the NameFactory class, which manages the progressive particle naming
 *     schemes.
 */

#include "name_factory.hpp"

// ================================================================================================
NameFactory::NameFactory()
{

}

// ================================================================================================
NameFactory::~NameFactory()
{

}

// ================================================================================================
bool NameFactory::hasName(const String& name)
{
	return (m_names.find(name) != m_names.end());
}

// ================================================================================================
void NameFactory::addName(const String& name)
{
	if (!hasName(name))
		return;

	m_names.insert(std::make_pair(name, uint32(0)));
}

// ================================================================================================
String NameFactory::getNext(const String& name)
{
	addName(name);
	return strfmt("%s%d", name.c_str(), (m_names[name]++));
}