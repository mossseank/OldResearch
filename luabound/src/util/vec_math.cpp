/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares functions related to manipulating rebound vectors.
 */

#include "vec_math.hpp"

namespace vecmath
{

reb_vec3d add(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

reb_vec3d sub(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

reb_vec3d add(const reb_vec3d& v1, double d)
{
	return {v1.x + d, v1.y + d, v1.z + d};
}

reb_vec3d sub(const reb_vec3d& v1, double d)
{
	return {v1.x - d, v1.y - d, v1.z - d};
}

reb_vec3d sub(double d, const reb_vec3d& v1)
{
	return {d - v1.x, d - v1.y, d - v1.z};
}

reb_vec3d mul(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

reb_vec3d div(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

reb_vec3d mul(const reb_vec3d& v1, double d)
{
	return {v1.x * d, v1.y * d, v1.z * d};
}

reb_vec3d div(const reb_vec3d& v1, double d)
{
	return {v1.x / d, v1.y / d, v1.z / d};
}

reb_vec3d div(double d, const reb_vec3d& v1)
{
	return {d / v1.x, d / v1.y, d / v1.z};
}

double len(const reb_vec3d& v1)
{
	return sqrt(dot(v1, v1));
}

double lensq(const reb_vec3d& v1)
{
	return dot(v1, v1);
}

double dot(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

reb_vec3d cross(const reb_vec3d& v1, const reb_vec3d& v2)
{
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.z
	};
}

reb_vec3d norm(const reb_vec3d& v1)
{
	return div(v1, len(v1));
}

} // namespace vecmath