/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the structure that is used in the Lua code to generate distributions
 *     and values for simulation parameters. These distributions match the reb_random_* functions
 *     present in the Rebound source code, and use these functions internally, to ensure maximum
 *     compliance with Rebound code.
 */
 
#ifndef LUABOUND_DISTRIBUTIONS_HPP_
#define LUABOUND_DISTRIBUTIONS_HPP_

#include "../luabound.hpp"

// Used to represent a distribution of values with the ability to generate random values from
//     the given distribution. Internally, uses a union to hold the various potential
//     distribution parameters, as well as the distribution type. "powerlaw" is the largest
//     union member, so in order to ensure zero-initialization you only need to set that.
struct value_distribution
{
public:
	enum disttype: uint8
	{
		DIST_TYPE_SINGULAR = 0,
		DIST_TYPE_UNIFORM = 1,
		DIST_TYPE_POWERLAW = 2,
		DIST_TYPE_NORMAL = 3,
		DIST_TYPE_RAYLEIGH = 4
	} type;
	union
	{
		struct
		{
			double value;
		} singular;
		struct
		{
			double min;
			double max;
		} uniform;
		struct
		{
			double min;
			double max;
			double slope;
		} powerlaw;
		struct
		{
			double mean;
			double variance;
		} normal;
		struct
		{
			double sigma;
		} rayleigh;
	};

public:
	value_distribution(double val = 0.0) :
		type{DIST_TYPE_SINGULAR}, powerlaw{val, 0.0, 0.0}
	{ }
	// The doubles that are passed are context sensitive to the type, under the following rules:
	//   type=SINGULAR: d1 = value, d2 = n/a,      d3 = n/a
	//   type=UNIFORM:  d1 = min,   d2 = max,      d3 = n/a
	//   type=POWERLAW: d1 = min,   d2 = max,      d3 = slope
	//   type=NORMAL:   d1 = mean,  d2 = variance, d3 = n/a
	//   type=RAYLEIGH: d1 = sigma, d2 = n/a,      d3 = n/a
	explicit value_distribution(disttype type, double d1 = 0.0, double d2 = 0.0, double d3 = 0.0) :
		type{type}, powerlaw{d1, d2, d3}
	{ }

	inline double getMin() const {
		return (type == DIST_TYPE_UNIFORM) ? uniform.min : (type == DIST_TYPE_POWERLAW) ? powerlaw.min : 0.0;
	}
	inline double getMax() const {
		return (type == DIST_TYPE_UNIFORM) ? uniform.max : (type == DIST_TYPE_POWERLAW) ? powerlaw.max : 0.0;
	}
	inline double getValue() const {
		return (type == DIST_TYPE_SINGULAR) ? singular.value : 0.0;
	}
	inline double getSlope() const {
		return (type == DIST_TYPE_POWERLAW) ? powerlaw.slope : 0.0;
	}
	inline double getMean() const {
		return (type == DIST_TYPE_NORMAL) ? normal.mean : 0.0;
	}
	inline double getVariance() const {
		return (type == DIST_TYPE_NORMAL) ? normal.variance : 0.0;
	}
	inline double getSigma() const {
		return (type == DIST_TYPE_RAYLEIGH) ? rayleigh.sigma : 0.0;
	}

	double generate() const;

	inline static bool FromLuaObject(sol::object& obj, value_distribution *dist) {
		if (obj.is<value_distribution>()) {
			*dist = obj.as<value_distribution>();
			return true;
		}
		if (obj.get_type() == sol::type::number) {
			*dist = value_distribution(obj.as<double>());
			return true;
		}

		return false;
	}
};

// Functions for turning lua function calls into distributions, and other luainterop function registration
namespace luainterop
{

extern void RegisterDistributionGlobals(sol::state& lua); // Register distribution related globals with the state

extern value_distribution CreateSingularDistribution(double val);
extern value_distribution CreateSingularDistributionNoArg();

extern value_distribution CreateUniformDistribution(double min, double max);
extern value_distribution CreateUniformDistributionMaxOnly(double max);
extern value_distribution CreateUniformDistributionNoArg();

extern value_distribution CreatePowerlawDistribution(double min, double max, double slope);
extern value_distribution CreatePowerlawDistributionNoArg();

extern value_distribution CreateNormalDistribution(double mean, double variance);
extern value_distribution CreateNormalDistributionVarianceOnly(double variance);
extern value_distribution CreateNormalDistributionNoArg();

extern value_distribution CreateRayleighDistribution(double sigma);
extern value_distribution CreateRayleighDistributionNoArg();

} // namespace luainterop

#endif // LUABOUND_DISTRIBUTIONS_HPP_