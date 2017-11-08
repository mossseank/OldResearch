/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements functions that parse lua tables for integrator parameters.
 */

#include "integrator_parser.hpp"

namespace integ_parse
{

bool IAS15(sol::table& integ, reb_simulation_integrator_ias15 *ias15)
{
	// Roll over each of the entries
	bool result = true;
	integ.for_each([&result, ias15] (sol::object key, sol::object value) -> void {
		if (!result) // If we have already failed, cut out of the parsing early
			return;

		String keyStr;
		if (key.get_type() != sol::type::string) {
			lerr("Keys in the 'integrator' table must be strings.");
			result = false;
			return;
		}
		else {
			keyStr = key.as<String>();
		}

		if (keyStr == "min_dt") {
			if (value.get_type() != sol::type::number) {
				lerr("The min_dt value must be a number.");
				result = false;
				return;
			}

			double mindt = value.as<double>();
			ias15->min_dt = mindt;
		}
		else if (keyStr == "epsilon") {
			if (value.get_type() != sol::type::number) {
				lerr("The epsilon value must be a number.");
				result = false;
				return;
			}

			double eps = value.as<double>();
			ias15->epsilon = eps;
		}
		else if (keyStr == "epsilon_global") {
			if (value.get_type() != sol::type::boolean) {
				lerr("The epsilon_global value must be a boolean.");
				result = false;
				return;
			}

			bool epsg = value.as<bool>();
			ias15->epsilon_global = epsg ? 1 : 0;
		}
		else if (keyStr != "name") {
			lwarn(strfmt("The integrator parameter '%s' is not a valid parameter for IAS15, and was ignored.",
				keyStr.c_str()));
		}
	});
	return result;
}

} // namespace integ_parse