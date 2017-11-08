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

#include "particle_manager.hpp"

// ================================================================================================
ParticleManager::ParticleManager(reb_simulation *sim) :
	m_sim{sim},
	m_hashNameMap{},
	m_nameHashMap{},
	m_primaryParticle{nullptr}
{

}

// ================================================================================================
ParticleManager::~ParticleManager()
{
	m_hashNameMap.clear();
	m_nameHashMap.clear();
}

// ================================================================================================
reb_particle* ParticleManager::addParticle(const String& name, reb_particle part)
{
	auto it = m_nameHashMap.find(name);
	if (it != m_nameHashMap.end()) {
		lwarn(strfmt("Overwriting particle with name '%s' with new particle.", name.c_str()));
		removeParticleByName(name, nullptr);
	}

	reb_add(m_sim, part);
	reb_particle *pt = &(m_sim->particles[m_sim->N - 1]);
	m_hashNameMap.insert(std::make_pair(pt->hash, name));
	m_nameHashMap.insert(std::make_pair(name, pt->hash));
	return pt;
}

// ================================================================================================
void ParticleManager::removeParticleByName(const String& name, reb_particle *out)
{
	auto it = m_nameHashMap.find(name);
	if (it == m_nameHashMap.end()) {
		if (out) 
			out->m = -1;
		return;
	}

	reb_particle *part = reb_get_particle_by_hash(m_sim, it->second);
	if (part) {
		if (out)
			*out = *part;
		reb_remove_by_hash(m_sim, it->second, 1);

		if (m_primaryParticle == part)
			m_primaryParticle = nullptr;
	}
	m_nameHashMap.erase(name);
	m_hashNameMap.erase(it->second);
}

// ================================================================================================
void ParticleManager::removeParticleByHash(uint32 hash, reb_particle *out)
{
	reb_particle *part = reb_get_particle_by_hash(m_sim, hash);
	if (part) {
		if (out)
			*out = *part;
		reb_remove_by_hash(m_sim, hash, 1);

		if (m_primaryParticle == part)
			m_primaryParticle = nullptr;
	}
	else if (out)
		out->m = -1;

	auto it = m_hashNameMap.find(hash);
	if (it == m_hashNameMap.end())
		return;
	
	m_hashNameMap.erase(hash);
	m_nameHashMap.erase(it->second);
}

// ================================================================================================
void ParticleManager::removeParticleName(uint32 hash)
{
	reb_particle *part = reb_get_particle_by_hash(m_sim, hash);
	if (part) {
		if (m_primaryParticle == part)
			m_primaryParticle = nullptr;
	}
	else
		return;

	auto it = m_hashNameMap.find(hash);
	if (it == m_hashNameMap.end())
		return;
	
	m_hashNameMap.erase(hash);
	m_nameHashMap.erase(it->second);
}

// ================================================================================================
reb_particle* ParticleManager::getParticleByName(const String& name)
{
	auto it = m_nameHashMap.find(name);
	if (it == m_nameHashMap.end())
		return nullptr;

	reb_particle* part = reb_get_particle_by_hash(m_sim, it->second);
	return part;
}

// ================================================================================================
reb_particle* ParticleManager::getParticleByHash(uint32 hash)
{
	reb_particle* part = reb_get_particle_by_hash(m_sim, hash);
	return part;
}

// ================================================================================================
String ParticleManager::getNameFromHash(uint32 hash)
{
	auto it = m_hashNameMap.find(hash);
	if (it != m_hashNameMap.end())
		return it->second;
	return "INVALID";
}

// ================================================================================================
bool ParticleManager::setPrimaryParticle(const reb_particle * const ref)
{
	if (ref == nullptr) {
		m_primaryParticle = nullptr;
		return true;
	}

	reb_particle *part = getParticleByHash(ref->hash);
	if (part == nullptr) {
		lerr(strfmt("Could not set primary particle reference with hash %d.", part->hash));
		return false;
	}
	m_primaryParticle = part;
	return true;
}

// ================================================================================================
bool ParticleManager::setPrimaryParticle(const String& name)
{
	reb_particle *part = getParticleByName(name);
	if (part == nullptr)
		return false;
	m_primaryParticle = part;
	return true;
}

// ================================================================================================
bool ParticleManager::setPrimaryParticle(uint32 hash)
{
	reb_particle *part = getParticleByHash(hash);
	if (part == nullptr)
		return false;
	m_primaryParticle = part;
	return true;
}

// ================================================================================================
int ParticleManager::getOrbitForParticle(const reb_particle * const part, reb_orbit& orbit)
{
	int err = 0;
	if (m_primaryParticle)
		orbit = reb_tools_particle_to_orbit_err(m_sim->G, *part, *m_primaryParticle, &err);
	else
		orbit = reb_tools_particle_to_orbit_err(m_sim->G, *part, reb_get_com(m_sim), &err);
	
	return err;
}