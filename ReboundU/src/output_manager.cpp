/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#include <output_manager.hpp>
#include <ctime>
#include <string.h>
#include <rebu_error.hpp>
#include <iostream>


namespace
{

// ================================================================================================
time_t _getRawTime()
{
	time_t rawtime;
	time(&rawtime);
	return rawtime;
}

// ================================================================================================
std::string _getFormattedTime(const std::string& format)
{
	time_t rawtime = _getRawTime();
	struct tm * nowtime = localtime(&rawtime);
	char outstr[128];
	if (!strftime(outstr, 128, format.c_str(), nowtime)) {
		return "INVALID_TIME";
	}
	return outstr;
}

}


// ================================================================================================
OutputFile::OutputFile(reb_simulation *sim, const std::string& file, double time) :
	m_sim{sim},
	m_format{nullptr},
	m_fileName{file},
	m_formatString{""},
	m_time{time},
	m_lastOutTime{0},
	m_fileHandle{nullptr},
	m_firstRun{true},
	m_isStdOut{file.length() == 0}
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
rebu_error_code_t OutputFile::loadFormat(const std::string& fmt)
{
	m_formatString = fmt;
	if(!m_format->loadFormat(fmt))
		return REBU_ERROR_INVALID_FORMAT;

	if (!m_isStdOut) {
		m_fileHandle->open(m_fileName.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (m_fileHandle->fail()) {
			_rebu_set_last_error_message("Could not open output file \"%s\" for writing, reason: (%d) \"%s\".", 
				m_fileName.c_str(), errno, strerror(errno));
			return REBU_ERROR_FILE_NOT_OPENED;
		}

		(*m_fileHandle) << "# filename: " << m_fileName << "\n"
						<< "# timestamp: " << _getFormattedTime("%d/%m/%y %H:%M:%S") << "\n"
						<< "# output timing: " << m_time << "\n"
						<< "# format: " << m_formatString << std::endl;
	}

	return REBU_ERROR_NONE;
}

// ================================================================================================
void OutputFile::update()
{
	const bool needsUpdate = (m_time < 0.0) 
			|| ((m_sim->t - m_lastOutTime) >= m_time)
			|| m_firstRun;

	if (needsUpdate)
	{
		std::stringstream outstr{""};
		m_format->generateOutput(m_sim, outstr);

		if (m_isStdOut)
			std::cout << outstr.str() << std::endl;
		else
			(*m_fileHandle) << outstr.str() << std::endl;

		m_lastOutTime = m_sim->t;
	}
	m_firstRun = false;
}


// ================================================================================================
OutputManager::OutputManager(reb_simulation *sim) :
	m_sim{sim},
	m_files{}
{

}

// ================================================================================================
OutputManager::~OutputManager()
{
	m_files.clear();
}

// ================================================================================================
rebu_error_code_t OutputManager::addOutput(const std::string& file, const std::string& format, double time)
{
	OutputFile *outFile = new OutputFile(m_sim, file, time);
	rebu_error_code_t err = outFile->loadFormat(format);
	if (err != REBU_ERROR_NONE) {
		delete outFile;
		return err;
	}
	m_files.push_back(std::shared_ptr<OutputFile>(outFile));

	return REBU_ERROR_NONE;
}

// ================================================================================================
void OutputManager::update()
{
	for (auto& file : m_files)
		file->update();
}