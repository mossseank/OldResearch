/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares functions related to manipulating rebound vectors.
 */

#ifndef VEC_MATH_HPP_
#define VEC_MATH_HPP_

#include "../luabound.hpp"

namespace vecmath
{

reb_vec3d add(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d sub(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d add(const reb_vec3d& v1, double d);
reb_vec3d sub(const reb_vec3d& v1, double d);
reb_vec3d sub(double d, const reb_vec3d& v1);
reb_vec3d mul(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d div(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d mul(const reb_vec3d& v1, double d);
reb_vec3d div(const reb_vec3d& v1, double d);
reb_vec3d div(double d, const reb_vec3d& v1);

double len(const reb_vec3d& v1);
double lensq(const reb_vec3d& v1);
double dot(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d cross(const reb_vec3d& v1, const reb_vec3d& v2);
reb_vec3d norm(const reb_vec3d& v1);

} // namespace vecmath

#endif // VEC_MATH_HPP_