/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file implements the OutputManager class, which handles parsing of the simulation file output
 *     settings, as well as the file output that happens at runtime.
 */

#include "output_manager.hpp"
#include "../simulation.hpp"
#include "../../util/clock.hpp"


// ================================================================================================
OutputFile::OutputFile(LbdSimulation *sim, const String& file, double time) :
	m_sim{sim},
	m_format{nullptr},
	m_fileName{file},
	m_formatString{""},
	m_time{time},
	m_lastOutTime{0},
	m_fileHandle{nullptr},
	m_firstRun{true},
	m_isStdOut{file.find("stdout") == 0}
{
	m_format = new OutputFormat;

	if (!m_isStdOut)
		m_fileHandle = new std::ofstream;
}

// ================================================================================================
OutputFile::~OutputFile()
{
	delete m_format;
	if (!m_isStdOut) {
		if (m_fileHandle->is_open())
			m_fileHandle->close();
		delete m_fileHandle;
	}
}

// ================================================================================================
bool OutputFile::loadFormat(const String& fmt)
{
	m_formatString = fmt;
	return m_format->loadFormat(fmt);
}

// ================================================================================================
bool OutputFile::update()
{
	const bool needsUpdate = (m_time < 0.0) 
			|| ((m_sim->getSimulation()->t - m_lastOutTime) >= m_time)
			|| m_firstRun;

	if (m_firstRun && !m_isStdOut) {
		m_fileHandle->open(m_fileName.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (m_fileHandle->fail()) {
			lerr(strfmt("Could not open output file \"%s\" for writing, reason: (%d) \"%s\".", 
				m_fileName.c_str(), errno, strerror(errno)));
			return false;
		}

		(*m_fileHandle) << "# filename: " << m_fileName << "\n"
						<< "# timestamp: " << Clock::GetFormattedTime(Clock::TIMEFMT_LONG) << "\n"
						<< "# output timing: " << m_time << "\n"
						<< "# format: " << m_formatString << std::endl;
	}
	m_firstRun = false;

	if (needsUpdate)
	{
		StringStream outstr{""};
		m_format->generateOutput(m_sim, outstr);

		if (m_isStdOut)
			lsim(outstr.str());
		else
			(*m_fileHandle) << outstr.str() << std::endl;

		m_lastOutTime = m_sim->getSimulation()->t;
	}

	return true;
}


// ================================================================================================
OutputManager::OutputManager(LbdSimulation *sim) :
	m_sim{sim},
	m_files{}
{

}

// ================================================================================================
OutputManager::~OutputManager()
{

}

// ================================================================================================
bool OutputManager::loadOutput(sol::table& table)
{
	bool good = true;

	table.for_each([&good, this](sol::object key, sol::object value) -> void {
		if (!good)
			return; // Break early if we have already encountered an error

		// Validate the key type
		if (key.get_type() != sol::type::string) {
			lerr("The keys for the output table must be strings, formatted as [\"name\"] = ...");
			good = false;
			return;
		}
		String fileName = key.as<String>();

		// Validate the value type
		if (!value.is<sol::table>()) {
			lerr(strfmt("The values in the output table \"%s\" must be output file specifier tables.", fileName.c_str()));
			good = false;
			return;
		}
		sol::table valueTable = value.as<sol::table>();

		// Extract time value
		sol::object tableObject;
		if ((tableObject = valueTable["time"]) == sol::nil) {
			lerr(strfmt("A time must be specified for the output file \"%s\".", fileName.c_str()));
			good = false;
			return;
		}
		if (tableObject.get_type() != sol::type::number) {
			lerr(strfmt("The time for output file \"%s\" must be specified as a number.", fileName.c_str()));
			good = false;
			return;
		}
		double fileTime = tableObject.as<double>();

		// Extract the format string
		if ((tableObject = valueTable["format"]) == sol::nil) {
			lerr(strfmt("A format must be specified for the output file \"%s\".", fileName.c_str()));
			good = false;
			return;
		}
		if (tableObject.get_type() != sol::type::string) {
			lerr(strfmt("The format for output file \"%s\" must be specified as a string.", fileName.c_str()));
			good = false;
			return;
		}
		String fileFormat = tableObject.as<String>();

		OutputFile *outFile = new OutputFile(m_sim, fileName, fileTime);
		good = outFile->loadFormat(fileFormat);
		if (good) {
			m_files.push_back(StlSharedPtr<OutputFile>(outFile));
			if (outFile->isStdOut())
				linfo(strfmt("Loaded terminal output with format \"%s\".", fileFormat.c_str()));
			else
				linfo(strfmt("Loaded output file \"%s\" with format \"%s\".", fileName.c_str(), fileFormat.c_str()));
		}
		else
			lerr(strfmt("Could not load the format string for output file \"%s\".", fileName.c_str()));
	});

	return good;
}

// ================================================================================================
bool OutputManager::update()
{
	bool good = true;

	for (auto& file : m_files)
		good = good && file->update();

	return good;
}