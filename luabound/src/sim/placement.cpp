/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implemets the different structures used to define the placement of simulation bodies
 *     within the simulation space. It also implements the lua interop functions for creating 
 *     these positional definitions.
 */

#include "placement.hpp"
#include "../runtime/simulation.hpp" // This is a gross coupling problem, find a way around this

// ================================================================================================
void cart_body_placement::generate(sim_particle_ref refpart, double mass, double *vals) const
{
	vals[0] = m_x.generate();
	vals[1] = m_y.generate();
	vals[2] = m_z.generate();
	vals[3] = m_vx.generate();
	vals[4] = m_vy.generate();
	vals[5] = m_vz.generate();
	vals[6] = m_ax.generate();
	vals[7] = m_ay.generate();
	vals[8] = m_az.generate();
}

// ================================================================================================
void spherical_body_placement::generate(sim_particle_ref refpart, double mass, double *vals) const
{
	const double r = m_r.generate();
	const double t = m_t.generate();
	const double p = m_p.generate();
	const double vr = m_vr.generate();
	const double vt = m_vt.generate();
	const double vp = m_vp.generate();
	const double ar = m_ar.generate();
	const double at = m_at.generate();
	const double ap = m_ap.generate();

	// Precalculate commonly used values
	const double SP = sin(p);
	const double CP = cos(p);
	const double ST = sin(t);
	const double CT = cos(t);
	const double SPST = SP * ST;
	const double SPCT = SP * CT;
	const double CPST = CP * ST;
	const double CPCT = CP * CT;
	const double VRVT = vr * vt;
	const double VRVP = vr * vp;
	const double RAT = r * at;
	const double RAP = r * ap;
	const double RVP2 = r * vp * vp;
	const double RVT2 = r * vt * vt;

	// Position
	vals[0] = r * SPCT;
	vals[1] = r * SPST;
	vals[2] = r * CP;

	// Velocity
	vals[3] = (SPCT * vr) + (r * SPST * vt) + (r * CPCT * vp);
	vals[4] = (SPST * vr) - (r * SPCT * vt) + (r * CPST * vp);
	vals[5] = (CP * vr) - (r * SP * vp);

	// Acceleration
	vals[6] = (ar * SPCT)   - (VRVP * CPCT) - (VRVT * SPST)
			- (VRVP * CPCT) - (RVP2 * SPCT) - (RAP * CPCT)
			- (VRVT * SPST) - (RVT2 * SPCT) - (RAT * SPST)
			+ (2 * r * vp * vt * CPST);
	vals[7] = (ar * SPST)   - (VRVP * CPST) + (VRVT * SPCT)
			- (VRVP * CPST) - (RVP2 * SPST) - (RAP * CPST)
			+ (VRVT * SPCT) - (RVT2 * SPST) + (RAT * SPCT)
			- (2 * r * vp * vt * CPCT);
	vals[8] = (ar * CP) + (2 * vp * vt * SP) - (RVP2 * CP) + (RAP * SP);
}

// ================================================================================================
void kepler_body_placement::generate(sim_particle_ref refpart, double mass, double *vals) const
{
	const double G = LbdSimulation::GetInstance()->getSimulation()->G;
	const double a = m_a.generate();
	const double e = m_e.generate();
	const double i = m_i.generate();
	const double O = m_O.generate();
	const double o = m_o.generate();
	const double f = m_f.generate();

	if (!(refpart.get())) {
		lerr("Kepler orbit generation requires a non-nil reference body.");
		throw "Logic Error";
	}

	int err = 0;
	reb_particle outpart = reb_tools_orbit_to_particle_err(G, *(refpart.get()), mass, a, e, i, O, o, f, &err);
	if (err) {
		String errmsg = (err == 1) ? "Can't set e exactly to 1" :
						(err == 2) ? "Eccentricity cannot be less than 0" :
						(err == 3) ? "Bound orbit (a > 0) cannot have e > 1" :
						(err == 4) ? "Unbound orbit (a < 0) cannot have e < 1" :
						(err == 5) ? "Unbound orbit can’t have f set beyond the asymptotes defining the particle" :
						"Unknown";
		lerr(strfmt("Kepler orbit generation error: '%s'.", errmsg.c_str()));
		throw "Logic Error";
	}
	
	vals[0] = outpart.x;
	vals[1] = outpart.y;
	vals[2] = outpart.z;
	vals[3] = outpart.vx;
	vals[4] = outpart.vy;
	vals[5] = outpart.vz;
	vals[6] = outpart.ax;
	vals[7] = outpart.ay;
	vals[8] = outpart.az;
}

// ================================================================================================
void pal_body_placement::generate(sim_particle_ref refpart, double mass, double *vals) const
{
	const double G = LbdSimulation::GetInstance()->getSimulation()->G;
	const double a = m_a.generate();
	const double l = m_l.generate();
	const double k = m_k.generate();
	const double h = m_h.generate();
	const double ix = m_ix.generate();
	const double iy = m_iy.generate();

	if (!(refpart.get())) {
		lerr("Pal orbit generation requires a non-nil reference body.");
		throw "Logic Error";
	}

	reb_particle outpart = reb_tools_pal_to_particle(G, *(refpart.get()), mass, a, l, k, h, ix, iy);

	vals[0] = outpart.x;
	vals[1] = outpart.y;
	vals[2] = outpart.z;
	vals[3] = outpart.vx;
	vals[4] = outpart.vy;
	vals[5] = outpart.vz;
	vals[6] = outpart.ax;
	vals[7] = outpart.ay;
	vals[8] = outpart.az;
}

namespace luainterop
{

// ================================================================================================
void RegisterPlacementGlobals(sol::state& lua)
{
	// Register the placement usertype under a mangled name to encourage users to not try to
	//     construct it directly
	lua.new_usertype<body_placement_ref>("__body_placement_",
		"new", sol::no_constructor,
		"generate", sol::overload(
			&body_placement_ref::luaGenerateNoArgs,
			&body_placement_ref::luaGenerate
		)
	);

	lua["place"] = lua.create_table_with(
		"cartesian", sol::overload(
			CreateCartesianNoArgs,
			CreateCartesianPos,
			CreateCartesianPosVel,
			CreateCartesianPosVelAcc
		),
		"spherical", sol::overload(
			CreateSphericalNoArgs,
			CreateSphericalPos,
			CreateSphericalPosVel,
			CreateSphericalPosVelAcc
		),
		"kepler2d", sol::overload(
			CreateKeplerNoArgs,
			CreateKepler2D
		),
		"kepler3d", sol::overload(
			CreateKeplerNoArgs,
			CreateKepler3D
		),
		"pal", sol::overload(
			CreatePalNoArgs,
			CreatePal
		)
	);
}

// ================================================================================================
body_placement_ref CreateCartesianNoArgs()
{
	return body_placement_ref(new cart_body_placement());
}

// ================================================================================================
body_placement_ref CreateCartesianPos(sol::object x, sol::object y, sol::object z)
{
	value_distribution x2, y2, z2;
	if (!value_distribution::FromLuaObject(x, &x2) || !value_distribution::FromLuaObject(y, &y2) 
			|| !value_distribution::FromLuaObject(z, &z2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new cart_body_placement(x2, y2, z2));
}

// ================================================================================================
body_placement_ref CreateCartesianPosVel(sol::object x, sol::object y, sol::object z,
										 sol::object vx, sol::object vy, sol::object vz)
{
	value_distribution x2, y2, z2, vx2, vy2, vz2;
	if (!value_distribution::FromLuaObject(x, &x2) || !value_distribution::FromLuaObject(y, &y2)
			|| !value_distribution::FromLuaObject(z, &z2) || !value_distribution::FromLuaObject(vx, &vx2)
			|| !value_distribution::FromLuaObject(vy, &vy2) || !value_distribution::FromLuaObject(vz, &vz2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new cart_body_placement(x2, y2, z2, vx2, vy2, vz2));
}

// ================================================================================================
body_placement_ref CreateCartesianPosVelAcc(sol::object x, sol::object y, sol::object z,
											sol::object vx, sol::object vy, sol::object vz,
											sol::object ax, sol::object ay, sol::object az)
{
	value_distribution x2, y2, z2, vx2, vy2, vz2, ax2, ay2, az2;
	if (!value_distribution::FromLuaObject(x, &x2) || !value_distribution::FromLuaObject(y, &y2)
			|| !value_distribution::FromLuaObject(z, &z2) || !value_distribution::FromLuaObject(vx, &vx2)
			|| !value_distribution::FromLuaObject(vy, &vy2) || !value_distribution::FromLuaObject(vz, &vz2)
			|| !value_distribution::FromLuaObject(ax, &ax2) || !value_distribution::FromLuaObject(ay, &ay2)
			|| !value_distribution::FromLuaObject(az, &az2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}
	return body_placement_ref(new cart_body_placement(x2, y2, z2, vx2, vy2, vz2, ax2, ay2, az2));
}

// ================================================================================================
body_placement_ref CreateSphericalNoArgs()
{
	return body_placement_ref(new spherical_body_placement());
}

// ================================================================================================
body_placement_ref CreateSphericalPos(sol::object r, sol::object t, sol::object p)
{
	value_distribution r2, t2, p2;
	if (!value_distribution::FromLuaObject(r, &r2) || !value_distribution::FromLuaObject(t, &t2)
			|| !value_distribution::FromLuaObject(p, &p2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new spherical_body_placement(r2, t2, p2));
}

// ================================================================================================
body_placement_ref CreateSphericalPosVel(sol::object r, sol::object t, sol::object p,
									 sol::object vr, sol::object vt, sol::object vp)
{
	value_distribution r2, t2, p2, vr2, vt2, vp2;
	if (!value_distribution::FromLuaObject(r, &r2) || !value_distribution::FromLuaObject(t, &t2)
			|| !value_distribution::FromLuaObject(p, &p2) || !value_distribution::FromLuaObject(vr, &vr2)
			|| !value_distribution::FromLuaObject(vt, &vt2) || !value_distribution::FromLuaObject(vp, &vp2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new spherical_body_placement(r2, t2, p2, vr2, vt2, vp2));
}

// ================================================================================================
body_placement_ref CreateSphericalPosVelAcc(sol::object r, sol::object t, sol::object p,
										sol::object vr, sol::object vt, sol::object vp,
										sol::object ar, sol::object at, sol::object ap)
{
	value_distribution r2, t2, p2, vr2, vt2, vp2, ar2, at2, ap2;
	if (!value_distribution::FromLuaObject(r, &r2) || !value_distribution::FromLuaObject(t, &t2)
			|| !value_distribution::FromLuaObject(p, &p2) || !value_distribution::FromLuaObject(vr, &vr2)
			|| !value_distribution::FromLuaObject(vt, &vt2) || !value_distribution::FromLuaObject(vp, &vp2)
			|| !value_distribution::FromLuaObject(ar, &ar2) || !value_distribution::FromLuaObject(at, &at2)
			|| !value_distribution::FromLuaObject(ap, &ap2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new spherical_body_placement(r2, t2, p2, vr2, vt2, vp2, ar2, at2, ap2));
}

// ================================================================================================
body_placement_ref CreateKeplerNoArgs()
{
	return body_placement_ref(new kepler_body_placement());
}

// ================================================================================================
body_placement_ref CreateKepler2D(sol::object a, sol::object e, sol::object o, sol::object f)
{
	value_distribution a2, e2, o2, f2;
	if (!value_distribution::FromLuaObject(a, &a2) || !value_distribution::FromLuaObject(e, &e2)
			|| !value_distribution::FromLuaObject(o, &o2) || !value_distribution::FromLuaObject(f, &f2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new kepler_body_placement(a2, e2, o2, f2));
}

// ================================================================================================
body_placement_ref CreateKepler3D(sol::object a, sol::object e, sol::object i, sol::object O,
								  sol::object o, sol::object f)
{
	value_distribution a2, e2, i2, O2, o2, f2;
	if (!value_distribution::FromLuaObject(a, &a2) || !value_distribution::FromLuaObject(e, &e2)
			|| !value_distribution::FromLuaObject(i, &i2) || !value_distribution::FromLuaObject(O, &O2)
			|| !value_distribution::FromLuaObject(o, &o2) || !value_distribution::FromLuaObject(f, &f2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new kepler_body_placement(a2, e2, i2, O2, o2, f2));
}

// ================================================================================================
body_placement_ref CreatePalNoArgs()
{
	return body_placement_ref(new pal_body_placement());
}

// ================================================================================================
body_placement_ref CreatePal(sol::object a, sol::object l, sol::object k, sol::object h,
							 sol::object ix, sol::object iy)
{
	value_distribution a2, l2, k2, h2, ix2, iy2;
	if (!value_distribution::FromLuaObject(a, &a2) || !value_distribution::FromLuaObject(l, &l2)
			|| !value_distribution::FromLuaObject(k, &k2) || !value_distribution::FromLuaObject(h, &h2)
			|| !value_distribution::FromLuaObject(ix, &ix2) || !value_distribution::FromLuaObject(iy, &iy2)) {
		lerr("Placement arguments must either be numbers or distributions.");
		throw "Logic Error";
	}

	return body_placement_ref(new pal_body_placement(a2, l2, k2, h2, ix2, iy2));
}

} // namespace luainterop