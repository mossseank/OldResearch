/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the ParticleFactory class, which manages the creation of particles
 *     from the lua script, before they are placed into the simulation. It manages the generation
 *     of coordinates, as well as the naming of particles.
 */

#ifndef LUABOUND_PARTICLE_FACTORY_HPP_
#define LUABOUND_PARTICLE_FACTORY_HPP_

#include "../../luabound.hpp"
#include "../../sim/particle.hpp"
#include "../../sim/distributions.hpp"
#include "../../sim/placement.hpp"
#include "name_factory.hpp"

class ParticleFactory
{
private:
	reb_simulation *m_sim;
	NameFactory *m_names;
	uint32 m_lastHash;

public:
	ParticleFactory(reb_simulation *sim);
	~ParticleFactory();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(ParticleFactory)

	// Returned mass = -1 on error, the passed name will the changed to the final particle name
	reb_particle createParticle(sol::object& mass, sol::object& radius, sol::object& place, 
		sol::object& refpart, String& name, bool multi);

private:
	bool parseMass(sol::object& mass, value_distribution *outmass);
	bool parseRadius(sol::object& radius, value_distribution *outradius);
	bool parsePlacement(sol::object& place, body_placement_ref *outplace);
	bool parseRefPart(sol::object& refpart, sim_particle_ref *outrefpart);
};

#endif // LUABOUND_PARTICLE_FACTORY_HPP_