--
-- This file is licensed under the MIT license, the text of which can be found in the README file in
--     this repository, or online at <https://opensource.org/licenses/MIT>.
-- Copyright (c) 2017 Sean Moss
--

-- Evolve for 50 orbits
local SIM_TIME = math.pi * 2 * 50
-- Output the particle information immediately before the sim ends
local OUT_TIME = SIM_TIME * 0.99999

-- The mass of the central black hole
local BH_MASS = 1.0
-- The number of particles to add
local PARTICLE_AMT = 1000
-- The mass ratio of the central object to the disk
local MASS_RATIO = 1e-3

new_simulation {

	name = "nuclear_disk",

	constants = {
		G = 1,
		max_time = SIM_TIME
	},

	plugins = {
		
	},

	integrator = {
		name = "ias15",
		min_dt = 1e-8
	},

	output = {
		["nuclear_disk.dat"] = {
			format = "#st,#sc,{#pa,#pe,#pi,#pO,#po,}",
			time = OUT_TIME
		}
	},

	populate = function()
		-- Add the black hole
		sim.addParticle(BH_MASS, 0, place.cartesian(0, 0, 0), nil, "blackhole")

		-- Create the disk placement
		local diskplace = place.kepler3d(
			dist.uniform(0.8, 1.2), 		-- a
			0.7, 							-- e
			dist.normal(1e-3), 				-- i
			0.0, 							-- Omega, long. of asc. node
			dist.normal(2e-2), 				-- omega, arg. of peri.
			dist.uniform(0, 10 * math.pi) 	-- f
		)

		-- Add the particles
		local PART_MASS = (BH_MASS * MASS_RATIO) / PARTICLE_AMT
		local bh = sim.getParticle("blackhole")
		sim.addParticles(PARTICLE_AMT, PART_MASS, 0, diskplace, bh, "star")
	end

}