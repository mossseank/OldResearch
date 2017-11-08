/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <format_token.hpp>
#include <vec_math.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>


namespace
{

int _getOrbitForParticle(reb_simulation *sim, const reb_particle *part, reb_orbit& orbit)
{
	int err = 0;
	orbit = reb_tools_particle_to_orbit_err(sim->G, *part, reb_get_com(sim), &err);
	return err;
}

#define VT_STRING (0)
#define VT_DOUBLE (1)
#define VT_INT (2)

#define SIMEXT_(token, value) case ValueSType::token: { ss << (value); break; }
void _printSimulationValue(reb_simulation *sim, ValueSType type, std::stringstream& ss)
{
	switch (type)
	{
		//SIMEXT_(Name, sim->getSimulationName())
		SIMEXT_(Time, sim->t)
		SIMEXT_(Lastdt, sim->dt_last_done)
		SIMEXT_(PCount, sim->N)
		//SIMEXT_(IName, sim->getIntegratorName())
		SIMEXT_(Gravity, sim->G)
		//SIMEXT_(TimeStep, sim->getTimestepCount())
		//SIMEXT_(WallTime, sim->getElapsedWallTime())
		//SIMEXT_(WallRes, Timer::GetResolution())
		default: ss << "INVALID"; break;
	}
}
#undef SIMEXT_

#define PARTEXT_(token, value) case ValuePType::token: { out << (value); break; }
#define PARTOEXT_(token, omember) case ValuePType::token: { \
	reb_orbit orbit; \
	int err = _getOrbitForParticle(sim, &part, orbit); \
	if (err) { \
		std::cerr << "Could not get the orbital value, reason: \"" << orbitErrMsg[err] << "\"." << std::endl; \
		throw "Orbit value get error."; \
	} \
	out << (orbit.omember); \
	break; \
}
const char *orbitErrMsg[2] = {
	"The particle has no mass.",
	"The particle is in the same place as the primary particle."
};
void _printParticleValue(reb_simulation *sim, std::uint32_t index, ValuePType type, std::stringstream& out)
{
	static const auto getEccentricityVector = [sim](const reb_particle& part) -> reb_vec3d {
		using namespace vecmath;
		const reb_vec3d pos{part.x, part.y, part.z};
		const reb_vec3d vel{part.vx, part.vy, part.vz};
		const double mu = sim->G * part.m;

		const double c1 = (lensq(vel) / mu) - (1 / len(pos));
		const double c2 = dot(pos, vel) / mu;

		const reb_vec3d v1 = mul(pos, c1);
		const reb_vec3d v2 = mul(vel, c2);
		return sub(v1, v2);
	};
	static const auto getAngMomVector = [sim](const reb_particle& part) -> reb_vec3d {
		using namespace vecmath;
		const reb_vec3d pos{part.x, part.y, part.z};
		const reb_vec3d vel{part.vx, part.vy, part.vz};
		return cross(pos, vel);
	};

	const reb_particle& part = sim->particles[index];

	switch (type) {
		PARTEXT_(Mass, part.m)
		PARTEXT_(Radius, part.r)
		//PARTEXT_(Name, sim->getManager()->getNameFromHash(part.hash));
		PARTEXT_(Hash, part.hash)
		PARTOEXT_(SMA, a)
		PARTOEXT_(Eccen, e)
		PARTOEXT_(Incl, inc)
		PARTOEXT_(LAN, Omega)
		PARTOEXT_(AP, omega)
		PARTOEXT_(TrueAnom, f)
		PARTOEXT_(MeanAnom, M)
		PARTEXT_(PosX, part.x)
		PARTEXT_(PosY, part.y)
		PARTEXT_(PosZ, part.z)
		PARTEXT_(VelX, part.vx)
		PARTEXT_(VelY, part.vy)
		PARTEXT_(VelZ, part.vz)
		PARTEXT_(AccX, part.ax)
		PARTEXT_(AccY, part.ay)
		PARTEXT_(AccZ, part.az)
		PARTEXT_(Distance, sqrt(part.x * part.x + part.y * part.y + part.z * part.z))
		PARTOEXT_(PDistance, d)
		PARTEXT_(EccX, getEccentricityVector(part).x)
		PARTEXT_(EccY, getEccentricityVector(part).y)
		PARTEXT_(EccZ, getEccentricityVector(part).z)
		case ValuePType::EccVec: {
			const reb_vec3d ecc = getEccentricityVector(part);
			out << "{{" << ecc.x << "|" << ecc.y << "|" << ecc.z << "}}";
			break;
		}
		PARTOEXT_(AngMom, h)
		PARTEXT_(AMX, getAngMomVector(part).x)
		PARTEXT_(AMY, getAngMomVector(part).y)
		PARTEXT_(AMZ, getAngMomVector(part).z)
		case ValuePType::AMVec: {
			const reb_vec3d am = getAngMomVector(part);
			out << "{{" << am.x << "|" << am.y << "|" << am.z << "}}";
			break;
		}
		default: out << "INVALID"; break;
	}
}
#undef PARTEXT_
#undef PARTOEXT_

#define PARTEXT_(token, value) case ValuePType::token: { vals[0] = (value); break; }
#define PARTOEXT_(token, omember) case ValuePType::token: { \
	reb_orbit orbit; \
	int err = _getOrbitForParticle(sim, &part, orbit); \
	if (err) { \
		std::cerr << "Could not get the orbital value, reason: \"" << orbitErrMsg[err] << "\"." << std::endl; \
		throw "Orbit value get error."; \
	} \
	vals[0] = (orbit.omember); \
	break; \
}
void _extractParticleValues(reb_simulation *sim, ValuePType type, double *vals)
{
	static const auto getEccentricityVector = [sim](const reb_particle& part) -> reb_vec3d {
		using namespace vecmath;
		const reb_vec3d pos{part.x, part.y, part.z};
		const reb_vec3d vel{part.vx, part.vy, part.vz};
		const double mu = sim->G * part.m;

		const double c1 = (lensq(vel) / mu) - (1 / len(pos));
		const double c2 = dot(pos, vel) / mu;

		const reb_vec3d v1 = mul(pos, c1);
		const reb_vec3d v2 = mul(vel, c2);
		return sub(v1, v2);
	};
	static const auto getAngMomVector = [sim](const reb_particle& part) -> reb_vec3d {
		using namespace vecmath;
		const reb_vec3d pos{part.x, part.y, part.z};
		const reb_vec3d vel{part.vx, part.vy, part.vz};
		return cross(pos, vel);
	};
	
	static const auto extractValue = [sim](const reb_particle& part, ValuePType type, double *vals) -> void {
		switch (type) {
			PARTEXT_(Mass, part.m)
			PARTEXT_(Radius, part.r)
			PARTOEXT_(SMA, a)
			PARTOEXT_(Eccen, e)
			PARTOEXT_(Incl, inc)
			PARTOEXT_(LAN, Omega)
			PARTOEXT_(AP, omega)
			PARTOEXT_(TrueAnom, f)
			PARTOEXT_(MeanAnom, M)
			PARTEXT_(PosX, part.x)
			PARTEXT_(PosY, part.y)
			PARTEXT_(PosZ, part.z)
			PARTEXT_(VelX, part.vx)
			PARTEXT_(VelY, part.vy)
			PARTEXT_(VelZ, part.vz)
			PARTEXT_(AccX, part.ax)
			PARTEXT_(AccY, part.ay)
			PARTEXT_(AccZ, part.az)
			PARTEXT_(Distance, sqrt(part.x * part.x + part.y * part.y + part.z * part.z))
			PARTOEXT_(PDistance, d)
			PARTEXT_(EccX, getEccentricityVector(part).x)
			PARTEXT_(EccY, getEccentricityVector(part).y)
			PARTEXT_(EccZ, getEccentricityVector(part).z)
			case ValuePType::EccVec: {
				const reb_vec3d ecc = getEccentricityVector(part);
				vals[0] = ecc.x;
				vals[1] = ecc.y;
				vals[2] = ecc.z;
				break;
			}
			PARTOEXT_(AngMom, h)
			PARTEXT_(AMX, getAngMomVector(part).x)
			PARTEXT_(AMY, getAngMomVector(part).y)
			PARTEXT_(AMZ, getAngMomVector(part).z)
			case ValuePType::AMVec: {
				const reb_vec3d am = getAngMomVector(part);
				vals[0] = am.x;
				vals[1] = am.y;
				vals[2] = am.z;
				break;
			}
			default: break;
		}
	};

	const int PCOUNT = sim->N;
	const reb_particle *PARTS = sim->particles;
	const int MULTIPLIER = (type == ValuePType::EccVec || type == ValuePType::AMVec) ? 3 : 1;
	for (int i = 0; i < PCOUNT; ++i) {
		extractValue(PARTS[i], type, &vals[i * MULTIPLIER]);
	}
}
#undef PARTEXT_
#undef PARTOEXT_

} // namespace 


namespace format_ast
{

// ================================================================================================
void punctuation_node::generateOutput(reb_simulation *sim, std::stringstream& out)
{
	out << pstring;
}

// ================================================================================================
void pvalue_token_node::generateOutput(reb_simulation *sim, std::stringstream& out)
{
	if (valueGroup == ValueGroup::Particle)
		_printParticleValue(sim, pIndex, valueType, out);
	else {
		const int PCOUNT = sim->N;
		const bool ISVEC = (valueType == ValuePType::EccVec || valueType == ValuePType::AMVec);
		double *vals = new double[PCOUNT * (ISVEC ? 3 : 1)];
		_extractParticleValues(sim, valueType, vals);

		if (ISVEC) {
			double sumx = 0, sumy = 0, sumz = 0;
			for (int i = 0; i < PCOUNT; ++i) {
				const int IDX = i * 3;
				sumx += vals[IDX + 0];
				sumy += vals[IDX + 1];
				sumz += vals[IDX + 2];
			}
			const double MEANX = sumx / PCOUNT;
			const double MEANY = sumy / PCOUNT;
			const double MEANZ = sumz / PCOUNT;

			if (valueGroup == ValueGroup::StdDev) {
				sumx = sumy = sumz = 0;
				for (int i = 0; i < PCOUNT; ++i) {
					const int IDX = i * 3;
					sumx += pow(vals[IDX + 0] - MEANX, 2);
					sumy += pow(vals[IDX + 1] - MEANY, 2);
					sumz += pow(vals[IDX + 2] - MEANZ, 2);
				}
				out << "{{" << sqrt(sumx / PCOUNT) << "|" << sqrt(sumy / PCOUNT) << "|" 
					<< sqrt(sumz / PCOUNT) << "}}";
			}
			else
				out << "{{" << MEANX << "|" << MEANY << "|" << MEANZ << "}}";
		}
		else {
			double sum = std::accumulate(&(vals[0]), &(vals[PCOUNT]), 0.0);
			const double MEAN = sum / PCOUNT;

			if (valueGroup == ValueGroup::StdDev) {
				sum = 0;
				for (int i = 0; i < PCOUNT; ++i) {
					sum += pow(vals[i] - MEAN, 2);
				}
				out << sqrt(sum / PCOUNT);
			}
			else
				out << MEAN;
		}

		delete vals;
	}
}

// ================================================================================================
void svalue_token_node::generateOutput(reb_simulation *sim, std::stringstream& out)
{
	_printSimulationValue(sim, valueType, out);
}

// ================================================================================================
void list_node::generateOutput(reb_simulation *sim, std::stringstream& out)
{
	const int pCount = sim->N;

	std::stringstream listss;
	for (int i = 0; i < pCount; ++i) {
		for (auto& node : nodeList) {
			node->pIndex = static_cast<std::uint32_t>(i);
			node->generateOutput(sim, listss);
		}
	}

	std::string liststr = listss.str();
	if (last) { // Trim the last punctuation token if necessary
		base_node *node = nodeList.at(nodeList.size() - 1).get();
		punctuation_node *pnode = nullptr;
		if ((pnode = dynamic_cast<punctuation_node*>(node)) != nullptr) {
			size_t len = pnode->pstring.length();
			liststr.erase(liststr.size() - len);
		}
	}

	out << liststr;
}

} // namespace format_ast


namespace token_utils
{

// ================================================================================================
#define STRVG_(name, token) if (str == name) { return ValueGroup::token; }
ValueGroup StringToValueGroup(const std::string& str)
{
	STRVG_("a", Average)
	else STRVG_("d", StdDev)
	else STRVG_("p", Particle)
	else STRVG_("s", Simulation)
	else return ValueGroup::INVALID;
}
#undef STRVG_

// ================================================================================================
#define STRVPT_(name, token) if (str == name) { return ValuePType::token; }
ValuePType StringToValuePType(const std::string& str)
{
	STRVPT_("m", Mass)
	else STRVPT_("r", Radius)
	else STRVPT_("n", Name)
	else STRVPT_("h", Hash)
	else STRVPT_("a", SMA)
	else STRVPT_("e", Eccen)
	else STRVPT_("i", Incl)
	else STRVPT_("O", LAN)
	else STRVPT_("o", AP)
	else STRVPT_("f", TrueAnom)
	else STRVPT_("M", MeanAnom)
	else STRVPT_("x", PosX)
	else STRVPT_("y", PosY)
	else STRVPT_("z", PosZ)
	else STRVPT_("vx", VelX)
	else STRVPT_("vy", VelY)
	else STRVPT_("vz", VelZ)
	else STRVPT_("ax", AccX)
	else STRVPT_("ay", AccY)
	else STRVPT_("az", AccZ)
	else STRVPT_("R", Distance)
	else STRVPT_("Rc", PDistance)
	else STRVPT_("ex", EccX)
	else STRVPT_("ey", EccY)
	else STRVPT_("ez", EccZ)
	else STRVPT_("ev", EccVec)
	else STRVPT_("j", AngMom)
	else STRVPT_("jx", AMX)
	else STRVPT_("jy", AMY)
	else STRVPT_("jz", AMZ)
	else STRVPT_("jv", AMVec)
	else return ValuePType::INVALID;
}
#undef STRVPT_

// ================================================================================================
#define STRVST_(name, token) if (str == name) { return ValueSType::token; }
ValueSType StringToValueSType(const std::string& str)
{
	STRVST_("n", Name)
	else STRVST_("t", Time)
	else STRVST_("dt", Lastdt)
	else STRVST_("c", PCount)
	else STRVST_("i", IName)
	else STRVST_("G", Gravity)
	else STRVST_("ts", TimeStep)
	else STRVST_("w", WallTime)
	else STRVST_("wr", WallRes)
	else return ValueSType::INVALID;
}
#undef STRVST_

// ================================================================================================
#define TTSTR_(token, name) case TokenType::token: return name;
std::string TokenTypeToString(TokenType type)
{
	switch (type) {
		TTSTR_(Punctuation, "Punctuation")
		TTSTR_(ValueToken, "Value Token")
		TTSTR_(ListSpecifier, "List Specifier")
		default: return "INVALID";
	}
}
#undef TTSTR_

// ================================================================================================
#define VGSTR_(token, name) case ValueGroup::token: return name;
std::string ValueGroupToString(ValueGroup grp)
{
	switch (grp) {
		VGSTR_(Average, "Global Average")
		VGSTR_(StdDev, "Global Standard Deviation")
		VGSTR_(Particle, "Particle")
		VGSTR_(Simulation, "Simulation")
		default: return "INVALID";
	}
}
#undef VGSTR_

// ================================================================================================
#define VPTSTR_(token, name) case ValuePType::token: return name;
std::string ValuePTypeToString(ValuePType type)
{
	switch (type) {
		VPTSTR_(Mass, "Mass")
		VPTSTR_(Radius, "Radius")
		VPTSTR_(Name, "Name")
		VPTSTR_(Hash, "Hash")
		VPTSTR_(SMA, "Semi-Major Axis")
		VPTSTR_(Eccen, "Eccentricity")
		VPTSTR_(Incl, "Inclination")
		VPTSTR_(LAN, "Longitude of Ascending Node")
		VPTSTR_(AP, "Argument of Pericenter")
		VPTSTR_(TrueAnom, "True Anomaly")
		VPTSTR_(MeanAnom, "Mean Anomaly")
		VPTSTR_(PosX, "X Position")
		VPTSTR_(PosY, "Y Position")
		VPTSTR_(PosZ, "Z Position")
		VPTSTR_(VelX, "X Velocity")
		VPTSTR_(VelY, "Y Velocity")
		VPTSTR_(VelZ, "Z Velocity")
		VPTSTR_(AccX, "X Acceleration")
		VPTSTR_(AccY, "Y Acceleration")
		VPTSTR_(AccZ, "Z Acceleration")
		VPTSTR_(Distance, "Distance from Origin")
		VPTSTR_(PDistance, "Distance from Primary Particle")
		VPTSTR_(EccX, "X Eccentricity Vector Component")
		VPTSTR_(EccY, "Y Eccentricity Vector Component")
		VPTSTR_(EccZ, "Z Eccentricity Vector Component")
		VPTSTR_(EccVec, "3-Component Eccentricity Vector")
		VPTSTR_(AngMom, "Magnitude of Angular Momentum")
		VPTSTR_(AMX, "X Angular Momentum Vector Component")
		VPTSTR_(AMY, "Y Angular Momentum Vector Component")
		VPTSTR_(AMZ, "Z Angular Momentum Vector Component")
		VPTSTR_(AMVec, "3-Component Angular Momentum Vector")
		default: return "INVALID";
	}
}
#undef VPTSTR_

// ================================================================================================
#define VSTSTR_(token, name) case ValueSType::token: return name;
std::string ValueSTypeToString(ValueSType type)
{
	switch (type) {
		VSTSTR_(Name, "Name")
		VSTSTR_(Time, "Current Time")
		VSTSTR_(Lastdt, "Last dt")
		VSTSTR_(PCount, "Particle Count")
		VSTSTR_(IName, "Integrator Name")
		VSTSTR_(Gravity, "Gravitational Constant")
		VSTSTR_(TimeStep, "Current Timestep")
		VSTSTR_(WallTime, "Current Wall Time")
		VSTSTR_(WallRes, "Wall Time Resolution")
		default: return "INVALID";
	}
}
#undef VSTSTR_

// ================================================================================================
ValueDataType GetSValueDataType(ValueSType type)
{
	switch (type) {
		case ValueSType::Name:
		case ValueSType::IName:
		case ValueSType::INVALID:
			return ValueDataType::String;
		case ValueSType::PCount:
			return ValueDataType::Int;
		case ValueSType::TimeStep:
			return ValueDataType::Long;
		default:
			return ValueDataType::Double;
	}
}

// ================================================================================================
ValueDataType GetPValueDataType(ValuePType type)
{
	switch (type) {
		case ValuePType::Name:
		case ValuePType::INVALID:
			return ValueDataType::String;
		case ValuePType::Hash:
			return ValueDataType::Int;
		default:
			return ValueDataType::Double;
	}
}

} // namespace token_utils