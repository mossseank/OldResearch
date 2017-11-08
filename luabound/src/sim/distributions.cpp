/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file is the implementation of the distributions.hpp file contents.
 */

#include "distributions.hpp"

// ================================================================================================
double value_distribution::generate() const
{
	switch (type) {
		case DIST_TYPE_UNIFORM: 
			return reb_random_uniform(uniform.min, uniform.max);
		case DIST_TYPE_POWERLAW:
			return reb_random_powerlaw(powerlaw.min, powerlaw.max, powerlaw.slope);
		case DIST_TYPE_NORMAL:
			return normal.mean + reb_random_normal(normal.variance);
		case DIST_TYPE_RAYLEIGH:
			return reb_random_rayleigh(rayleigh.sigma);
		case DIST_TYPE_SINGULAR: 
		default:
			return singular.value;
	}
}

namespace luainterop
{

// ================================================================================================
void RegisterDistributionGlobals(sol::state& lua)
{
	// Register the distribution usertype under a mangled name to encourage users to not try to
	//     construct it directly
	lua.new_usertype<value_distribution>("__value_distribution_",
		"new", sol::no_constructor,
		"generate", &value_distribution::generate,
		"type", sol::readonly(&value_distribution::type),
		"min", sol::readonly_property(&value_distribution::getMin),
		"max", sol::readonly_property(&value_distribution::getMax),
		"value", sol::readonly_property(&value_distribution::getValue),
		"slope", sol::readonly_property(&value_distribution::getSlope),
		"mean", sol::readonly_property(&value_distribution::getMean),
		"variance", sol::readonly_property(&value_distribution::getVariance),
		"sigma", sol::readonly_property(&value_distribution::getSigma)
	);

	lua["dist"] = lua.create_table_with(
		// Add the distruibution creation functions
		"singular", sol::overload(
			CreateSingularDistribution,
			CreateSingularDistributionNoArg
		),
		"uniform", sol::overload(
			CreateUniformDistribution,
			CreateUniformDistributionMaxOnly,
			CreateUniformDistributionNoArg
		),
		"power", sol::overload(
			CreatePowerlawDistribution,
			CreatePowerlawDistributionNoArg
		),
		"normal", sol::overload(
			CreateNormalDistribution,
			CreateNormalDistributionVarianceOnly,
			CreateNormalDistributionNoArg
		),
		"rayleigh", sol::overload(
			CreateRayleighDistribution,
			CreateRayleighDistributionNoArg
		),
		// Add the distribution types
		"type", lua.create_table_with(
			"singular", static_cast<double>(value_distribution::DIST_TYPE_SINGULAR),
			"uniform", static_cast<double>(value_distribution::DIST_TYPE_UNIFORM),
			"powerlaw", static_cast<double>(value_distribution::DIST_TYPE_POWERLAW),
			"normal", static_cast<double>(value_distribution::DIST_TYPE_NORMAL),
			"rayleigh", static_cast<double>(value_distribution::DIST_TYPE_RAYLEIGH)
		)
	);
}

// ================================================================================================
extern value_distribution CreateSingularDistribution(double val)
{
	return value_distribution(val);
}

// ================================================================================================
extern value_distribution CreateSingularDistributionNoArg()
{
	return value_distribution();
}

// ================================================================================================
value_distribution CreateUniformDistribution(double min, double max)
{
	return value_distribution(value_distribution::DIST_TYPE_UNIFORM, min, max);
}

// ================================================================================================
value_distribution CreateUniformDistributionMaxOnly(double max)
{
	return value_distribution(value_distribution::DIST_TYPE_UNIFORM, 0.0, max);
}

// ================================================================================================
value_distribution CreateUniformDistributionNoArg()
{
	return value_distribution(value_distribution::DIST_TYPE_UNIFORM);
}

// ================================================================================================
value_distribution CreatePowerlawDistribution(double min, double max, double slope)
{
	return value_distribution(value_distribution::DIST_TYPE_POWERLAW, min, max, slope);
}

// ================================================================================================
value_distribution CreatePowerlawDistributionNoArg()
{
	return value_distribution(value_distribution::DIST_TYPE_POWERLAW);
}

// ================================================================================================
value_distribution CreateNormalDistribution(double mean, double variance)
{
	return value_distribution(value_distribution::DIST_TYPE_NORMAL, mean, variance);
}

// ================================================================================================
value_distribution CreateNormalDistributionVarianceOnly(double variance)
{
	return value_distribution(value_distribution::DIST_TYPE_NORMAL, 0.0, variance);
}

// ================================================================================================
value_distribution CreateNormalDistributionNoArg()
{
	return value_distribution(value_distribution::DIST_TYPE_NORMAL);
}

// ================================================================================================
value_distribution CreateRayleighDistribution(double sigma)
{
	return value_distribution(value_distribution::DIST_TYPE_RAYLEIGH, sigma);
}

// ================================================================================================
value_distribution CreateRayleighDistributionNoArg()
{
	return value_distribution(value_distribution::DIST_TYPE_RAYLEIGH);
}

} // namespace luainterop