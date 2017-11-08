/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file declares the OutputManager class, which handles parsing of the simulation file output
 *     settings, as well as the file output that happens at runtime.
 */

#ifndef LUABOUND_OUTPUT_MANAGER_HPP_
#define LUABOUND_OUTPUT_MANAGER_HPP_

#include "../../luabound.hpp"
#include "format_parser.hpp"
#include <fstream>

// Forward declare LbdSimulation
class LbdSimulation;

class OutputFile
{
private:
	LbdSimulation *m_sim;
	OutputFormat *m_format;
	String m_fileName;
	String m_formatString;
	double m_time;
	double m_lastOutTime;
	std::ofstream *m_fileHandle;
	bool m_firstRun;
	const bool m_isStdOut;

public:
	OutputFile(LbdSimulation *sim, const String& file, double time);
	~OutputFile();

	bool isStdOut() const { return m_isStdOut; }

	bool loadFormat(const String& fmt);
	bool update();
};


class OutputManager
{
private:
	using FileList = StlVector<StlSharedPtr<OutputFile>>;

	LbdSimulation *m_sim;
	FileList m_files;

public:
	OutputManager(LbdSimulation *sim);
	~OutputManager();

	LUABOUND_DECLARE_CLASS_NONCOPYABLE(OutputManager)

	bool loadOutput(sol::table& table);
	bool update();
};

#endif // LUABOUND_OUTPUT_MANAGER_HPP_