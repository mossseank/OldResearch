/*
 * This file is licensed under the MIT license, the text of which can be found in the README file in
 *     this repository, or online at <https://opensource.org/licenses/MIT>.
 * Copyright(c) 2017 Sean Moss
 */

#pragma once
#define PARTICLE_HPP_

#include <vector>

namespace part
{

#pragma pack(push, 1)
struct Orbit
{
	float a;
	float e;
	float i;
	float O;
	float o;
};
#pragma pack(pop)
using OrbitList = std::vector<Orbit>;

#pragma pack(push, 1)
struct StarPos
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
};
#pragma pack(pop)

struct CalculatedStar
{
	int pointCount;
	size_t dataSize;
	Orbit orbit;
	StarPos *posList;

	CalculatedStar(const Orbit& orb) :
		pointCount{static_cast<int>(pow(orb.a, 1.5f))},
		dataSize{pointCount * sizeof(StarPos)},
		orbit{orb},
		posList{new StarPos[pointCount]}
	{ }

	~CalculatedStar() { delete posList; }
};
using StarList = std::vector<CalculatedStar>;


// Load the orbit parameters from the input file
extern bool loadOrbitInfo(const char* path, OrbitList& orbits);
// Generate the orbital points from the orbital data
extern bool generateOrbitPoints(const OrbitList& orbits, StarList& stars);

} // namespace part