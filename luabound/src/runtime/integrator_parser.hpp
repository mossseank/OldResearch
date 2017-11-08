/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares functions that parse lua tables for integrator parameters.
 */

#ifndef INTEGRATOR_PARSER_HPP_
#define INTEGRATOR_PARSER_HPP_

#include "../luabound.hpp"

namespace integ_parse
{

extern bool IAS15(sol::table& integ, reb_simulation_integrator_ias15 *ias15);

} // namespace integ_parse

#endif // INTEGRATOR_PARSER_HPP_