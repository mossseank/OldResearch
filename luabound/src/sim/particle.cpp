/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file defines the lua interop functions for creating particles, as well as the structure
 *     that allows lua code to access and directly manipulate rebound particles in the simulation.
 */

#include "particle.hpp"

namespace luainterop
{

// ================================================================================================
void RegisterParticleGlobals(sol::state& lua)
{
	// Register the particle ref usertype under a mangled name to encourage users to not try to
	//     construct it directly
	lua.new_usertype<sim_particle_ref>("__sim_particle_",
		"new", sol::no_constructor,
		"x", sol::property(&sim_particle_ref::getX, &sim_particle_ref::setX),
		"y", sol::property(&sim_particle_ref::getY, &sim_particle_ref::setY),
		"z", sol::property(&sim_particle_ref::getZ, &sim_particle_ref::setZ),
		"vx", sol::property(&sim_particle_ref::getVX, &sim_particle_ref::setVX),
		"vy", sol::property(&sim_particle_ref::getVY, &sim_particle_ref::setVY),
		"vz", sol::property(&sim_particle_ref::getVZ, &sim_particle_ref::setVZ),
		"ax", sol::property(&sim_particle_ref::getAX, &sim_particle_ref::setAX),
		"ay", sol::property(&sim_particle_ref::getAY, &sim_particle_ref::setAY),
		"az", sol::property(&sim_particle_ref::getAZ, &sim_particle_ref::setAZ),
		"m", sol::property(&sim_particle_ref::getMass, &sim_particle_ref::setMass),
		"r", sol::property(&sim_particle_ref::getRadius, &sim_particle_ref::setRadius),
		"hash", sol::readonly_property(&sim_particle_ref::getHash)
	);
}

} // namespace luainterop