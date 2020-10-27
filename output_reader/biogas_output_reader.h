/*
 * Copyright (c) 2020:  G-CSC, Goethe University Frankfurt
 * Author: Paul Zügel
 * 
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#include <string>
#include <vector>
#include "output_entry.h"

/**
 * Class to save all Data from outputFiles.lua
 *
 * Following parameters are used to communicate with LabView:
 *
 * @param number_of_lines_output: Number of total parameters
 * @param outputFilesTreeString: All information needed to construct the LabView tree (CSV-style string)
 * @param outputFilesPlotString: All information to plot the values (CSV-style string)
 *
 * Following parameters are internal:
 *
 * @param input: Input outputFiles.lua
 * @param input_modified: Modified input for easier parsing
 * @param entries: Internal container for all data
 */
class BiogasOutputReader { 
	public:
		int number_of_lines_output;

		std::string outputFilesTreeString;
		std::string outputFilesPlotString;

	private:
		std::string input; //original input whithout linebreaks
		std::string input_modified; //Formatted original input with linebreaks

		std::vector<OutputEntry> entries;

	public:
		BiogasOutputReader(){};
		bool init(const char*);	

	private:
		bool load(std::string);
		bool readOutputFiles();
		void generateTreeString();
		void generatePlotString();
		void modifyInput();
		void readXValues(std::vector<std::string>*, std::vector<std::string>*, std::vector<std::string>*);
};



