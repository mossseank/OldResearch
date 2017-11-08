/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the ParticleManager class, which is used by the LbdSimulation class to
 *     manage names and references of particles in its own reb_simulation. It also implements
 *     particle getters, setters, and search/access functionality.
 */

#ifndef LUABOUND_PARTICLE_MANAGER_HPP_
#define LUABOUND_PARTICLE_MANAGER_HPP_

#include "../../luabound.hpp"
#include "../../sim/particle.hpp"

class ParticleManager
{
public:
	using HashNameLookup = StlHashMap<uint32, String>;
	using NameHashLookup = StlHashMap<String, uint32>;

private:
	reb_simulation *m_sim;
	HashNameLookup m_hashNameMap;
	NameHashLookup m_nameHashMap;
	reb_particle* m_primaryParticle;

public:
	ParticleManager(reb_simulation *sim);
	~ParticleManager();

	reb_particle* addParticle(const String& name, reb_particle part);

	void removeParticleByName(const String& name, reb_particle *out);
	void removeParticleByHash(uint32 hash, reb_particle *out);

	void removeParticleName(uint32 hash); // Used by the collision callback

	reb_particle* getParticleByName(const String& name);
	reb_particle* getParticleByHash(uint32 hash);

	String getNameFromHash(uint32 hash);

	bool hasPrimaryParticle() const { return (m_primaryParticle != nullptr); }
	reb_particle* getPrimaryParticle() const { return m_primaryParticle; }
	bool setPrimaryParticle(const reb_particle * const ref);
	bool setPrimaryParticle(const String& name);
	bool setPrimaryParticle(uint32 hash);

	int getOrbitForParticle(const reb_particle * const part, reb_orbit& orbit); // Might move this elsewhere eventually
	
	LUABOUND_DECLARE_CLASS_NONCOPYABLE(ParticleManager)
};

#endif // LUABOUND_PARTICLE_MANAGER_HPP_