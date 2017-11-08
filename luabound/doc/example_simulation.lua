
-- This is a function call that creates the simulation specified by the below information
new_simulation {
	-- This is the name of the simulation (not quite sure what will be done with this yet)
	name = "example_simulation",

	-- These are the constants that can be set for the simulation
	constants = {
		G = 1, -- Set G = 1
		max_time = "inf" -- Set max time equal to infinity, but a number could have been specified
	},

	-- These are the plugins to load for the simulation
	plugins = {
		"example_plugin"
	},

	-- These are the integrator settings, both the name and the integrator-specific values
	integrator = {
		name = "ias15",
		min_dt = 1e-7 --,
		-- epsilon = 1e-9,
		-- epsilon_global = false
	},

	-- Define the output style of the program.
	--     The output will support any number of simultaneous output files, just be aware of
	--     performance issues with having many files open at once.
	output = {
		-- The first output file. This one tracks only the average eccentricity of the simulation
		["avg_e.dat"] = { -- Sets the output filename to be "avg_e.dat"
			format = "#ae", -- Format of output, in this case the global average e, one per line
			time = math.pi / 2.0 -- Create output every pi/2 times (every 1/4 orbit, with G=1, a=1 units)
		},
		-- The second output file, which creates a list of each particle's semi-major axis
		["all_a.dat"] = { -- Sets the output filename to be "all_a.dat"
			format = "#st, #sc: {#pa,#pe;}", -- Each line starts with the simulation time, particle count, then
											 --     a list of all semi-major axes and eccentricities for each particle
			time = math.pi / 2.0
		},
		-- Complex output to test the python parser file
		["parser_test.dat"] = {
			format = "#st, #sc: {#pa,#pe;} #sG, #ajv, #djv: {#pj,}",
			time = math.pi / 2.0
		},
		-- Output specified to go to stdout, instead of to a file
		["stdout0"] = {
			format = "#st",
			time = math.pi
		}
	},

	-- This is the function that is called immediately after new_simulation, which works to
	--    actually populate the simulation with the various bodies. This function is not called
	--    right away because the programmer may want to react to changing things, such as center
	--    of mass.
	populate = function()
		-- Particles are added to the simulation using two primary concepts, that of:
		-- 1) "Distributions":
		--        Distributions define random sets of numbers, using different mathematical
		--         distribution types. The possible types are `singular`, `uniform`, `power`,
		--         `normal`, and `rayleigh`. These are pretty self explanitory, except for maybe
		--         `singular`, which just represents an individual number and not a range. These
		--         distributions are available through the "dist" global table.
		-- 2) "Placements":
		--        Placements define different coordinate systems or elements that can be used
		--         to define locations for the simulation bodies. They all take a varying number
		--         of parameters, and the parameters can either be singular numbers, or one of
		--         the distribution types explained above. These placements are available through
		--         the "place" global table.

		-- Create a placement using 3D kepler orbital elements, which are (a, e, i, Omega, omega, f)
		local disk_place = place.kepler3d(
			dist.uniform(0.8, 1.2), -- For a, create a uniform distribution between 0.8 and 1.2 units
			0.7, -- Use a set eccentricity of 0.7 for all particles created with this placement
			dist.normal(1e-3), -- Use a normal (gaussian) distribution for inclination, mean=0, var=1e-3
			0.0, -- Use 0 for all longitude of ascending nodes
			0.0, -- Use 0 for all argument of pericenters
			dist.uniform(0, 2 * math.pi) -- Uniformly distribute the true anomalies between 0 and 2pi radians
		)

		-- Reference the active simulation using the global "sim" table.
		-- This function adds a single particle at (0,0,0) with mass 1, and radius 1e-4, with the "nil"
		--     meaning that the particle should be placed absolutely, not relatively to another particle.
		--     Finally, a name is specified so the particle can be grabbed in the next step.
		sim.addParticle(1, 1e-4, place.cartesian(0.0, 0.0, 0.0), nil, "central_object")

		-- This function gets the central object, which is needed to place kepler element orbits. This could
		--     also have worked using `local ref_part = sim.addParticle(...)` from above.
		local ref_part = sim.getParticle("central_object")

		-- This function adds multiple (50) particles using the placement defined above. They are all given the
		--     same mass (1e-3) and radius (1e-4), but these too could be specified with distributions, if wanted. The
		--     names are specified as "star{n}". They are all specified to be relative to the "central_object" particle 
		--     (this is required for kepler element orbital parameters).
		sim.addParticles(10, 1e-3, 1e-4, disk_place, ref_part, "star")
	end
}