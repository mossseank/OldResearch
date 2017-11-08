/*
 * This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
 *     found in the LICENSE file distributed with the ReboundU source code, or online at 
 *     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
 *     text of the license itself, must not be removed from the source files.
 * Copyright © 2017 Sean Moss
 */

#pragma once
#define OUTPUT_MANAGER_HPP_

#include "format_parser.hpp"
#include <fstream>


class OutputFile
{
private:
	reb_simulation *m_sim;
	OutputFormat *m_format;
	std::string m_fileName;
	std::string m_formatString;
	double m_time;
	double m_lastOutTime;
	std::ofstream *m_fileHandle;
	bool m_firstRun;
	const bool m_isStdOut;

public:
	OutputFile(reb_simulation *sim, const std::string& file, double time);
	~OutputFile();

	bool isStdOut() const { return m_isStdOut; }

	rebu_error_code_t loadFormat(const std::string& fmt);
	void update();
};


class OutputManager
{
private:
	using FileList = std::vector<std::shared_ptr<OutputFile>>;

	reb_simulation *m_sim;
	FileList m_files;

public:
	OutputManager(reb_simulation *sim);
	~OutputManager();

	rebu_error_code_t addOutput(const std::string& file, const std::string& format, double time);
	void update();
};