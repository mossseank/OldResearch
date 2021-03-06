/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares all of enumerations and functions needed to parse and store format string
 *     token information.
 */

#ifndef FORMAT_TOKEN_HPP_
#define FORMAT_TOKEN_HPP_

#include "../../luabound.hpp"

class OutputFormat;

// The type of token parsed from the format string
enum class TokenType :
	uint8
{
	Punctuation,
	ValueToken,
	ListSpecifier,
	INVALID
};

// The type of the value token (global average, global std.dev., particle, or simulation)
enum class ValueGroup :
	uint8
{
	Average,
	StdDev,
	Particle,
	Simulation,
	INVALID
};

// The subtype of value token, for the particle, average, and standard deviation groups
enum class ValuePType :
	uint8
{
	Mass,      // Mass (m)
	Radius,    // Radius (r)
	Name,      // Name (n)
	Hash,      // Hash (h)
	SMA,       // Semi-major axis (a)
	Eccen,     // Eccentricity (e)
	Incl,      // Inclination (i)
	LAN,       // Longitude of Ascending Node (O)
	AP,        // Argument of pericenter (o)
	TrueAnom,  // True anomaly (f)
	MeanAnom,  // Mean anomaly (M)
	PosX,      // X Position (x)
	PosY,      // Y Position (y)
	PosZ,      // Z Position (z)
	VelX,      // X Velocity (vx)
	VelY,      // Y Velocity (vy)
	VelZ,      // Z Velocity (vz)
	AccX,      // X Acceleration (ax)
	AccY,      // Y Acceleration (ay)
	AccZ,      // Z Acceleration (az)
	Distance,  // Distance from (0, 0, 0) (R)
	PDistance, // Distance from primary particle (Rc)
	EccX,      // X component of eccentricity vector (ex)
	EccY,      // Y component of eccentricity vector (ey)
	EccZ,      // Z component of eccentricity vector (ez)
	EccVec,    // 3-component eccentricity vector (ev)
	AngMom,    // Magnitude of angular momentum
	AMX,       // X component of angular momentum vector (jx)
	AMY,       // Y component of angular momentum vector (jy)
	AMZ,       // Z component of angular momentum vector (jz)
	AMVec,       // 3-component angular momentum vector (jv)
	INVALID
};

// The subtype of value token, for the simulation group
enum class ValueSType :
	uint8
{
	Name,     // Name of simulation (n)
	Time,     // Simulation time (t)
	Lastdt,   // Last dt done by simulation (dt)
	PCount,   // Particle count (c)
	IName,    // Integrator name (i)
	Gravity,  // Gravitational constant (G)
	TimeStep, // Current simulation timestep (ts)
	WallTime, // Current wall time since simulation started (w)
	WallRes, // Wall time timer precision (wr)
	INVALID
};

// The data type that the token represents
enum class ValueDataType :
	uint8
{
	String,
	Double,
	Int,
	Long
};


// Forward declaration of LbdSimulation class
class LbdSimulation;

namespace format_ast
{

struct base_node
{
public:
	uint32 pIndex; // Only used by pvalue nodes, but defined here for speed

public:
	base_node() :
		pIndex{0}
	{ }

	virtual void generateOutput(LbdSimulation *sim, StringStream& out) = 0;
};

struct punctuation_node :
	base_node
{
public:
	const String pstring;

public:
	punctuation_node(const String& pstr) :
		pstring{pstr}
	{ }

	void generateOutput(LbdSimulation *sim, StringStream& out) override;
};

struct pvalue_token_node :
	base_node
{
public:
	const ValueGroup valueGroup;
	const ValuePType valueType;

public:
	pvalue_token_node(ValueGroup vg, ValuePType vt) :
		valueGroup{vg}, valueType{vt}
	{ }

	void generateOutput(LbdSimulation *sim, StringStream& out) override;
};

struct svalue_token_node :
	base_node
{
public:
	const ValueSType valueType;

public:
	svalue_token_node(ValueSType vt) :
		valueType{vt}
	{ }

	void generateOutput(LbdSimulation *sim, StringStream& out) override;
};

struct list_node :
	base_node
{
public:
	using node_list = StlVector<StlUniquePtr<base_node>>;
	using node_ptr_list = StlVector<base_node*>;

	const node_list nodeList;
	const size_t count;
	const bool last; // If this is true, the trailing punctuation will be cut out of the last list item

public:
	list_node(node_ptr_list& list, bool lastn) :
		nodeList(std::move(constructVector(list))), count(list.size()), last{lastn}
	{ }

	void generateOutput(LbdSimulation *sim, StringStream& out) override;

private:
	static node_list constructVector(node_ptr_list& list)
	{
		node_list lst;
		for (auto p : list)
			lst.emplace_back(StlUniquePtr<base_node>(p));
		return lst;
	}
};

} // namespace format_ast


namespace token_utils
{

extern ValueGroup StringToValueGroup(const String& str);
extern ValuePType StringToValuePType(const String& str);
extern ValueSType StringToValueSType(const String& str);

extern String TokenTypeToString(TokenType type);
extern String ValueGroupToString(ValueGroup grp);
extern String ValuePTypeToString(ValuePType type);
extern String ValueSTypeToString(ValueSType type);

extern ValueDataType GetSValueDataType(ValueSType type);
extern ValueDataType GetPValueDataType(ValuePType type);

} // namespace token_utils

#endif // FORMAT_TOKEN_HPP_