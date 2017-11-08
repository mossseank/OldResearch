/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the ParticleFactory class, which manages the creation of particles
 *     from the lua script, before they are placed into the simulation. It manages the generation
 *     of coordinates, as well as the naming of particles.
 */

#include "particle_factory.hpp"

// ================================================================================================
ParticleFactory::ParticleFactory(reb_simulation *sim) :
	m_sim{sim},
	m_names{nullptr},
	m_lastHash{0}
{
	m_names = new NameFactory;
}

// ================================================================================================
ParticleFactory::~ParticleFactory()
{
	if (m_names)
		delete m_names;
}

// ================================================================================================
reb_particle ParticleFactory::createParticle(sol::object& mass, sol::object& radius, sol::object& place, 
	sol::object& refpart, String& name, bool multi)
{
	String oname = name;
	if (multi)
		name = m_names->getNext(oname);

	value_distribution pmass;
	value_distribution pradius;
	body_placement_ref pplace;
	sim_particle_ref prefpart;

	reb_particle part;

	if (!parseMass(mass, &pmass) || !parseRadius(radius, &pradius) 
			|| !parsePlacement(place, &pplace) || !parseRefPart(refpart, &prefpart)) {
		part.m = -1;
		return part;
	}

	part.m = pmass.generate();
	part.r = pradius.generate();

	double pos[9];
	pplace.generate(prefpart, part.m, pos);
	part.x = pos[0];
	part.y = pos[1];
	part.z = pos[2];
	part.vx = pos[3];
	part.vy = pos[4];
	part.vz = pos[5];
	part.ax = pos[6];
	part.ay = pos[7];
	part.az = pos[8];

	// Give it a unique hash
	part.hash = (m_lastHash++);

	return part;
}

// ================================================================================================
bool ParticleFactory::parseMass(sol::object& mass, value_distribution *outmass)
{
	if (!value_distribution::FromLuaObject(mass, outmass)) {
		lerr("Particle mass must be specified as a number or a distribution.");
		return false;	
	}
	return true;
}

// ================================================================================================
bool ParticleFactory::parseRadius(sol::object& radius, value_distribution *outradius)
{
	if (!value_distribution::FromLuaObject(radius, outradius)) {
		lerr("Particle radius must be specified as a number or a distribution.");
		return false;	
	}
	return true;
}

// ================================================================================================
bool ParticleFactory::parsePlacement(sol::object& place, body_placement_ref *outplace)
{
	if (!place.is<body_placement_ref>()) {
		lerr("Particle position must be specified as a placement.");
		return false;
	}

	*outplace = place.as<body_placement_ref>();
	return true;
}

// ================================================================================================
bool ParticleFactory::parseRefPart(sol::object& refpart, sim_particle_ref *outrefpart)
{
	if (refpart == sol::nil) {
		*outrefpart = sim_particle_ref();
		return true;
	}
	if (!refpart.is<sim_particle_ref>()) {
		lerr("A reference particle must be specified by a particle reference or nil.");
		return false;
	}

	*outrefpart = refpart.as<sim_particle_ref>();
	return true;
}