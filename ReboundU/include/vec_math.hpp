/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#pragma once
#define VEC_MATH_HPP_

#include <reboundu.h>


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