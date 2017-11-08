/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <reboundu.h>
#include <iostream>
#include <cmath>


// Simulation for this example is taken from parameters for an eccentric disk.

void report_output_error(const char *file, rebu_error_code_t err);
void heartbeat_callback(reb_simulation * const sim);


int main(int argc, char **argv)
{
	// Create simulation
	reb_simulation *sim = reb_create_simulation();

	// Setup simulation
	sim->G = 1;
	sim->integrator = reb_simulation::REB_INTEGRATOR_IAS15;
	sim->heartbeat = heartbeat_callback;

	// Add black hole
	reb_particle bh;
	bh.m = 1;
	reb_add(sim, bh);

	// Add the stars
	for (int ii = 0; ii < 25; ++ii) {
		double a = reb_random_uniform(1.1, 1.7);
		double e = reb_random_uniform(0.65, 0.75);
		double i = reb_random_normal(3e-2);
		double Omega = 0;
		double omega = 0;
		double f = reb_random_normal(M_PI / 4.0) + M_PI;

		reb_particle part = reb_tools_orbit_to_particle(1, bh, 1e-3, a, e, i, Omega, omega, f);
		reb_add(sim, part);
	}

	// Add the output files for ReboundU
	rebu_error_code_t err;
	if ((err = rebu_add_output_file(sim, "avg_e.dat", "#ae", M_PI / 2.0)) != REBU_ERROR_NONE) {
		report_output_error("avg_e.dat", err);
		return -1;
	}
	if ((err = rebu_add_output_file(sim, "full_info.dat", "#st, #sc: {#pa,#pe;} #sG, #ajv, #djv, {#pj,}", M_PI / 2.0)) 
			!= REBU_ERROR_NONE) {
		report_output_error("full_info.dat", err);
		return -1;
	}
	if ((err = rebu_add_terminal_output(sim, "#st", M_PI / 2.0)) != REBU_ERROR_NONE) {
		report_output_error("terminal output", err);
		return -1;
	}

	// Launch simulation
	reb_move_to_com(sim);
	reb_integrate(sim, INFINITY);

	// Clean and exit
	rebu_exit();
	reb_free_simulation(sim);
	return 0;
}

void heartbeat_callback(reb_simulation * const sim)
{
	// Allow ReboundU to update
	rebu_update();
}

void report_output_error(const char *file, rebu_error_code_t err)
{
	std::cout << "Could not open " << file << ", error (" << rebu_error_code_to_str(err) << "): "
		<< rebu_get_error_string(err) << std::endl;
	std::cout << "\tReason: '" << rebu_get_last_error_message() << "'" << std::endl;
}