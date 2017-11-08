/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the structure used to define the placement of simulation bodies
 *     within the simulation space. It also defines the lua interop functions for creating 
 *     these positional definitions.
 */

#ifndef LUABOUND_PLACEMENT_HPP_
#define LUABOUND_PLACEMENT_HPP_

#include "../luabound.hpp"
#include "distributions.hpp"
#include "particle.hpp"

// Base struct for body placement. Because lua has a reference to these, we can use polymorphism
//     to specify different placements, instead of context typing.
struct body_placement
{
private:
	bool m_strict; // If the placement requires a mass and reference particle to work.

public:
	body_placement(bool strict) :
		m_strict(strict)
	{ }

	inline bool isStrict() const { return m_strict; }

	// Generates the position, velocity, and acceleration information about this placement.
	// The array passed in must be of at least size 9, and will contain the information after
	//     calling as follows: {x, y, z, vx, vy, vz, ax, ay, az}.
	virtual void generate(sim_particle_ref refpart, double mass, double *vals) const = 0;

	void generate(double *vals) { generate(nullptr, 0.0, vals); }
};

// Structure for defining cartesian style coordinates (x, y, z).
struct cart_body_placement :
	public body_placement
{
private:
	value_distribution m_x;
	value_distribution m_y;
	value_distribution m_z;
	value_distribution m_vx;
	value_distribution m_vy;
	value_distribution m_vz;
	value_distribution m_ax;
	value_distribution m_ay;
	value_distribution m_az;

public:
	cart_body_placement() :
		body_placement(false),
		m_x{}, m_y{}, m_z{}, 
		m_vx{}, m_vy{}, m_vz{}, 
		m_ax{}, m_ay{}, m_az{}
	{ }
	cart_body_placement(const value_distribution& x, const value_distribution& y, const value_distribution& z) :
		body_placement(false),
		m_x{x}, m_y{y}, m_z{z}, 
		m_vx{}, m_vy{}, m_vz{}, 
		m_ax{}, m_ay{}, m_az{}
	{ }
	cart_body_placement(const value_distribution& x, const value_distribution& y, const value_distribution& z,
						const value_distribution& vx, const value_distribution& vy, const value_distribution& vz) :
		body_placement(false),
		m_x{x}, m_y{y}, m_z{z}, 
		m_vx{vx}, m_vy{vy}, m_vz{vz}, 
		m_ax{}, m_ay{}, m_az{}
	{ }
	cart_body_placement(const value_distribution& x, const value_distribution& y, const value_distribution& z,
						const value_distribution& vx, const value_distribution& vy, const value_distribution& vz,
						const value_distribution& ax, const value_distribution& ay, const value_distribution& az) :
		body_placement(false),
		m_x{x}, m_y{y}, m_z{z}, 
		m_vx{vx}, m_vy{vy}, m_vz{vz}, 
		m_ax{ax}, m_ay{ay}, m_az{az}
	{ }

	void generate(sim_particle_ref refpart, double mass, double *vals) const override;
};

// Structure for defining polar style coordinates (r, theta, phi).
// In this case, theta is longitudinal angle, and phi is polar angle
struct spherical_body_placement :
	public body_placement
{
private:
	value_distribution m_r;
	value_distribution m_t;
	value_distribution m_p;
	value_distribution m_vr;
	value_distribution m_vt;
	value_distribution m_vp;
	value_distribution m_ar;
	value_distribution m_at;
	value_distribution m_ap;

public:
	spherical_body_placement() :
		body_placement(false),
		m_r{}, m_t{}, m_p{},
		m_vr{}, m_vt{}, m_vp{},
		m_ar{}, m_at{}, m_ap{}
	{ }
	spherical_body_placement(const value_distribution& r, const value_distribution& t, const value_distribution& p) :
		body_placement(false),
		m_r{r}, m_t{t}, m_p{p},
		m_vr{}, m_vt{}, m_vp{},
		m_ar{}, m_at{}, m_ap{}
	{ }
	spherical_body_placement(const value_distribution& r, const value_distribution& t, const value_distribution& p,
						 const value_distribution& vr, const value_distribution& vt, const value_distribution& vp) :
		body_placement(false),
		m_r{r}, m_t{t}, m_p{p},
		m_vr{vr}, m_vt{vt}, m_vp{vp},
		m_ar{}, m_at{}, m_ap{}
	{ }
	spherical_body_placement(const value_distribution& r, const value_distribution& t, const value_distribution& p,
						 const value_distribution& vr, const value_distribution& vt, const value_distribution& vp,
						 const value_distribution& ar, const value_distribution& at, const value_distribution& ap) :
		body_placement(false),
		m_r{r}, m_t{t}, m_p{p},
		m_vr{vr}, m_vt{vt}, m_vp{vp},
		m_ar{ar}, m_at{at}, m_ap{ap}
	{ }

	void generate(sim_particle_ref refpart, double mass, double *vals) const override;
};

// Structure for defining kepler orbital parameters
struct kepler_body_placement :
	public body_placement
{
private:
	value_distribution m_a; // Semi-major axis
	value_distribution m_e; // Eccentricity
	value_distribution m_i; // Inclination
	value_distribution m_O; // Longitude of ascending node
	value_distribution m_o; // Argument of pericenter
	value_distribution m_f; // True anomaly

public:
	kepler_body_placement() :
		body_placement(true),
		m_a{}, m_e{}, m_i{},
		m_O{}, m_o{}, m_f{}
	{ }
	// 2d orbital parameters
	kepler_body_placement(const value_distribution& a, const value_distribution& e, const value_distribution& o,
						  const value_distribution& f) :
		body_placement(true),
		m_a{a}, m_e{e}, m_i{},
		m_O{}, m_o{o}, m_f{f}
	{ }
	// 3d orbital parameters
	kepler_body_placement(const value_distribution& a, const value_distribution& e, const value_distribution& i,
						  const value_distribution& O, const value_distribution& o, const value_distribution& f) :
		body_placement(true),
		m_a{a}, m_e{e}, m_i{i},
		m_O{O}, m_o{o}, m_f{f}
	{ }

	void generate(sim_particle_ref refpart, double mass, double *vals) const override;
};

// Structure for defining pal orbital parameters (Pal 2009)
struct pal_body_placement :
	public body_placement
{
private:
	value_distribution m_a; // Semi-major axis
	value_distribution m_l; // lambda = longitude
	value_distribution m_k; // Eccentricity/pericenter k = e*cos(w)
	value_distribution m_h; // Eccentricity/pericenter h = e*sin(w)
	value_distribution m_ix; // Inclination, x-component
	value_distribution m_iy; // Inclination, y-component

public:
	pal_body_placement() : 
		body_placement(true),
		m_a{}, m_l{}, m_k{},
		m_h{}, m_ix{}, m_iy{}
	{ }
	pal_body_placement(const value_distribution& a, const value_distribution& l, const value_distribution& k,
					   const value_distribution& h, const value_distribution& ix, const value_distribution& iy) :
		body_placement(true),
		m_a{a}, m_l{l}, m_k{k},
		m_h{h}, m_ix{ix}, m_iy{iy}
	{ }

	void generate(sim_particle_ref refpart, double mass, double *vals) const override;
};

// Lightweight object that is exposed to lua to hold body_placement references.
struct body_placement_ref
{
private:
	StlSharedPtr<body_placement> m_ref;

public:
	using generate_tuple = std::tuple<double, double, double,
									  double, double, double,
									  double, double, double>;

	body_placement_ref() :
		m_ref{new cart_body_placement()}
	{ }
	body_placement_ref(body_placement *dist) :
		m_ref{dist}
	{ }

	inline const body_placement& get() const { return *m_ref; }
	inline void generate(sim_particle_ref refpart, double mass, double *vals) const { m_ref->generate(refpart, mass, vals); }
	inline generate_tuple luaGenerateNoArgs() const {
		if (m_ref->isStrict()) {
			lerr("Kepler and Pal orbital placement requires arguments of the reference body and mass.");
			throw "Logic Error";
		}
		double vals[9];
		m_ref->generate(nullptr, 0.0, vals);
		return std::make_tuple(vals[0], vals[1], vals[2],
							   vals[3], vals[4], vals[5],
							   vals[6], vals[7], vals[8]);
	}
	inline generate_tuple luaGenerate(sol::object refpart, sol::object mass) const {
		if (!refpart.is<sim_particle_ref>()) {
			lerr("The object passed into the generate function of a placement must be a particle.");
			throw "Logic Error";
		}
		if (mass.get_type() != sol::type::number) {
			lerr("The mass passed to the generate function of a placement must be a number.");
			throw "Logic Error";
		}
		double vals[9];
		m_ref->generate(refpart.as<sim_particle_ref>(), mass.as<double>(), vals);
		return std::make_tuple(vals[0], vals[1], vals[2],
							   vals[3], vals[4], vals[5],
							   vals[6], vals[7], vals[8]);
	}
};


namespace luainterop
{

extern void RegisterPlacementGlobals(sol::state& lua);

extern body_placement_ref CreateCartesianNoArgs();
extern body_placement_ref CreateCartesianPos(sol::object x, sol::object y, sol::object z);
extern body_placement_ref CreateCartesianPosVel(sol::object x, sol::object y, sol::object z,
												sol::object vx, sol::object vy, sol::object vz);
extern body_placement_ref CreateCartesianPosVelAcc(sol::object x, sol::object y, sol::object z,
												   sol::object vx, sol::object vy, sol::object vz,
												   sol::object ax, sol::object ay, sol::object az);

extern body_placement_ref CreateSphericalNoArgs();
extern body_placement_ref CreateSphericalPos(sol::object r, sol::object t, sol::object p);
extern body_placement_ref CreateSphericalPosVel(sol::object r, sol::object t, sol::object p,
											sol::object vr, sol::object vt, sol::object vp);
extern body_placement_ref CreateSphericalPosVelAcc(sol::object r, sol::object t, sol::object p,
											   sol::object vr, sol::object vt, sol::object vp,
											   sol::object ar, sol::object at, sol::object ap);

extern body_placement_ref CreateKeplerNoArgs();
extern body_placement_ref CreateKepler2D(sol::object a, sol::object e, sol::object o, sol::object f);
extern body_placement_ref CreateKepler3D(sol::object a, sol::object e, sol::object i, sol::object O,
										 sol::object o, sol::object f);

extern body_placement_ref CreatePalNoArgs();
extern body_placement_ref CreatePal(sol::object a, sol::object l, sol::object k, sol::object h,
									sol::object ix, sol::object iy);

} // namespace luainterop

#endif // LUABOUND_PLACEMENT_HPP_