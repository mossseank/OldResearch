/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file defines the lua interop functions for creating particles, as well as the structure
 *     that allows lua code to access and directly manipulate rebound particles in the simulation.
 */

#ifndef LUABOUND_PARTICLE_HPP_
#define LUABOUND_PARTICLE_HPP_

#include "../luabound.hpp"

// Holds a reference to a reb_particle object, and allows property manipulation from lua code.
struct sim_particle_ref
{
private:
	reb_particle *m_ref;
	bool m_owned;

public:
	sim_particle_ref(reb_particle *p = nullptr, bool owned = false) : // Potentially bad, be mindful of object lifetimes
		m_ref{p},
		m_owned{owned}
	{ }
	sim_particle_ref(const sim_particle_ref& other) :
		m_ref{nullptr},
		m_owned{other.m_owned}
	{
		if (m_owned) {
			m_ref = new reb_particle;
			*m_ref = *(other.m_ref); // Copy construct
		}
		else
			m_ref = other.m_ref;
	}
	sim_particle_ref& operator = (const sim_particle_ref& other)
	{
		m_ref = nullptr;
		m_owned = other.m_owned;
		if (m_owned) {
			m_ref = new reb_particle;
			*m_ref = *(other.m_ref); // Copy construct
		}
		else
			m_ref = other.m_ref;
	}
	~sim_particle_ref()
	{
		if (m_owned && m_ref)
			delete m_ref;
	}

	reb_particle* get() { return m_ref; }

	inline double getX() { return m_ref->x; }
	inline double getY() { return m_ref->y; }
	inline double getZ() { return m_ref->z; }
	inline double getVX() { return m_ref->vx; }
	inline double getVY() { return m_ref->vy; }
	inline double getVZ() { return m_ref->vz; }
	inline double getAX() { return m_ref->ax; }
	inline double getAY() { return m_ref->ay; }
	inline double getAZ() { return m_ref->az; }
	inline double getMass() { return m_ref->m; }
	inline double getRadius() { return m_ref->r; }
	inline uint32 getHash() { return m_ref->hash; }

	inline void setX(double d) { m_ref->x = d; }
	inline void setY(double d) { m_ref->y = d; }
	inline void setZ(double d) { m_ref->z = d; }
	inline void setVX(double d) { m_ref->vx = d; }
	inline void setVY(double d) { m_ref->vy = d; }
	inline void setVZ(double d) { m_ref->vz = d; }
	inline void setAX(double d) { m_ref->ax = d; }
	inline void setAY(double d) { m_ref->ay = d; }
	inline void setAZ(double d) { m_ref->az = d; }
	inline void setMass(double d) { m_ref->m = d; }
	inline void setRadius(double d) { m_ref->r = d; }
};

namespace luainterop
{

extern void RegisterParticleGlobals(sol::state& lua);

} // namespace luainterop

#endif // LUABOUND_PARTICLE_HPP_