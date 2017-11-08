/*
 * This file is licensed under the MIT license, the text of which can be found in the README file in
 *     this repository, or online at <https://opensource.org/licenses/MIT>.
 * Copyright(c) 2017 Sean Moss
 */

#include <iostream>
#include <numeric>
#include "graphics.hpp"
#include "particle.hpp"

// Hardcoded values, eventually load these from command line args or config file
const unsigned int FB_RESOLUTION = 250;
const char * const INPUT_FILE_PATH = "../../sim/nuclear_disk.dat";


void cleanForExit(bool context = true, bool target = true);

int main(int argc, char **argv)
{
	std::cout << "Initializing Program..." << std::endl;
	// Create the render context
	GLFWwindow *window;
	if (!(window = grph::createRenderContext()))
		return -1;
	std::cout << "\tInitilized rendering context" << std::endl;

	// Create the render target
	grph::RenderTarget *rtarget;
	if (!(rtarget = grph::createRenderTarget(FB_RESOLUTION))) {
		cleanForExit(true, false);
		return -1;
	}
	std::cout << "\tInitialized render targets with resolution of " << rtarget->resolution << std::endl;

	std::cout << "Loading Orbit Data..." << std::endl;
	// Try to load the orbit data from the input file
	part::OrbitList orbits;
	if (!part::loadOrbitInfo(INPUT_FILE_PATH, orbits)) {
		cleanForExit();
		return -1;
	}
	std::cout << "\tLoaded data from input file (" << orbits.size() << " orbits)" << std::endl;
	int ecnt = std::accumulate(orbits.begin(), orbits.end(), 0,
		[](int acc, const part::Orbit& orb) -> int {
			std::cout << orb.e << std::endl;
			if (orb.e == 1.0f)
				return acc + 1;
			else
				return acc;
		}
	);
	std::cout << "e=1 count: " << ecnt << std::endl;

	// Generate the star positions from the orbital information
	part::StarList stars;
	if (!part::generateOrbitPoints(orbits, stars)) {
		cleanForExit();
		return -1;
	}
	int pointcount = std::accumulate(stars.begin(), stars.end(), 0, 
		[](int acc, const part::CalculatedStar& star) -> int {
			return acc + star.pointCount;
		}
	);
	std::cout << "\tGenerated star positions from orbital data (" << pointcount << " points)" << std::endl;

	// Attach event handlers
	auto _glfwKeyCallback = [](GLFWwindow *win, int key, int scancode, int action, int mods) -> void {
	
	};
	glfwSetKeyCallback(window, _glfwKeyCallback);

	while (!glfwWindowShouldClose(window))
	{
		// Poll window events
		glfwPollEvents();

		// Do the next render sequence
		glfwSwapBuffers(window);
	}

	cleanForExit();
	std::cout << "Exiting..." << std::endl;
	return 0;
}

void cleanForExit(bool context, bool target)
{
	if (context)
		grph::destroyRenderContext();
	if (target)
		grph::destroyRenderTarget();
}