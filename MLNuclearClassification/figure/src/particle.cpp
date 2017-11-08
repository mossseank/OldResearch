/*
 * This file is licensed under the MIT license, the text of which can be found in the README file in
 *     this repository, or online at <https://opensource.org/licenses/MIT>.
 * Copyright(c) 2017 Sean Moss
 */

#include "particle.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>


using FileStream = std::ifstream;

inline bool _fltEqual(float f1, float f2)
{
	const float MACHINE_EPSILON = std::numeric_limits<float>::epsilon();
	return fabs(f1 - f2) <= MACHINE_EPSILON;
}


namespace part
{

// ====================================================================================================================
bool loadOrbitInfo(const char* path, OrbitList& orbits)
{
	// Try to open the input file
	FileStream file(path);
	if (!file.is_open()) {
		const char* err = strerror(errno);
		std::cerr << "ERROR: Unable to open input file '" << path << "', reason: '" << err << "'." << std::endl;
		return false;
	}

	// Read in the four header lines
	std::string header[4];
	for (int i = 0; i < 4; ++i) {
		if (!std::getline(file, header[i])) {
			std::cerr << "ERROR: Could not read header line in input file." << std::endl;
			file.close();
			return false;
		}
		if (header[i].find('#') != 0) {
			std::cerr << "ERROR: The header line '" << header[i] << "' is not valid." << std::endl;
			file.close();
			return false;
		}
	}

	// Check format and warn if incorrect
	std::string format = header[3].substr(header[3].find_first_of(':') + 2);
	if (format != "#st,#sc,{#pa,#pe,#pi,#pO,#po,}") {
		std::cerr << "WARNING: the input file format does not match the expected format of "
				  << "'#st,#sc,{#pa,#pe,#pi,#pO,#po,}'." << std::endl;
	}

	// Read to the last non-empty line, and use that as the data line
	std::string dataline;
	for (std::string line; std::getline(file, line); ) {
		if (!line.empty())
			dataline = line;
	}
	if (dataline.empty()) {
		std::cerr << "ERROR: Could not find the orbit information in the input file." << std::endl;
		file.close();
		return false;
	}
	file.close();

	// Load particle count
	std::stringstream datastream;
	datastream.str(dataline);
	std::string timestr, countstr;
	if (!std::getline(datastream, timestr, ',') || !std::getline(datastream, countstr, ',')) {
		std::cerr << "ERROR: Could not load the simulation time and/or particle count from the input data." << std::endl;
		return false;
	}
	int pcount; 
	try {
		pcount = std::stoi(countstr) - 1; // -1 to skip the central object 
	}
	catch (...) {
		std::cerr << "ERROR: Count not parse the particle count from the orbit data." << std::endl;
		return false;
	}
	const int entrycount = pcount * 5;
	if (pcount <= 0) {
		std::cerr << "ERROR: Particle count was <= 1, must have at least one non-central-object orbit." << std::endl;
		return false;
	}

	// Burn through the first five entries, they are the central particle which we dont care about
	for (int i = 0; i < 5; ++i) {
		std::string str;
		if (!std::getline(datastream, str, ',')) {
			std::cerr << "ERROR: Malformatted orbit data, could not even find central object data." << std::endl;
			return false;
		}
		std::cout << "Skipped: " << str << std::endl;
	}

	// Reserve space for the orbit information, and split the input up into a list of floats
	orbits.clear();
	orbits.reserve(pcount);
	float *rawdata = new float[entrycount];
	int count = 0;
	for (std::string line; std::getline(datastream, line, ',') && count < entrycount; ++count) {
		try {
			rawdata[count] = std::stof(line);
		}
		catch (...) {
			std::cout << "ERROR: Could not parse floating point entry " << count << "." << std::endl;
			delete rawdata;
			return false;
		}
	}
	if (count != entrycount) {
		std::cerr << "ERROR: Expected " << entrycount << " floating point number entries in input, only got " << count
				  << "." << std::endl;
		delete rawdata;
		return false;
	}

	// Populate the particle information
	for (int i = 0; i < pcount; ++i) {
		const int INDEX = i * 5;
		orbits.push_back({
			rawdata[INDEX],
			rawdata[INDEX + 1],
			rawdata[INDEX + 2],
			rawdata[INDEX + 3],
			rawdata[INDEX + 4]
		});
	}
	delete rawdata;

	return true;
}

// ====================================================================================================================
bool generateOrbitPoints(const OrbitList& orbits, StarList& stars)
{
	// Validate all of the orbital elements before we try to generate them
	int index = 0;
	for (const Orbit& orb : orbits) {
		if (_fltEqual(orb.e, 1.0f)) {
			std::cerr << "ERROR: Cannot generate perfectly radial (e=1) orbits from kepler elements (p " << index <<
					  ")." << std::endl;
			return false;
		}
		if (orb.e < 0.0f) {
			std::cerr << "ERROR: Cannot have a negative eccentricity (p " << index << ")." << std::endl;
			return false;
		}
		if (orb.e > 1.0f && orb.a > 0.0f) {
			std::cerr << "ERROR: A bound orbit (a > 0) must have e < 1 (p " << index << ")." << std::endl;
			return false;
		}
		if (orb.e <= 1.0f && orb.a < 0.0f) {
			std::cerr << "ERROR: An unbound orbit (a < 0) must have e > 1 (p " << index << ")." << std::endl;
			return false;
		}
		++index;
	}

	return true;
}

} // namespace part