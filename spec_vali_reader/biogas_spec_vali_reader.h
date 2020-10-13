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

#include "table_entry.h"
#include <string>
#include <vector>

class BiogasSpecValiReader { 
	public:
		int number_of_entries;

		std::string validationErrors;
		std::string validationMessage;
		std::string outputString;
		std::string valiString;
		std::string specString;

	private:
		std::string input;
		std::string input_valiModified;
		std::string input_specModified;

		std::vector<TableEntry> entries;

	public:
		BiogasSpecValiReader(){};	
		bool init_Vali(const char* filepath_vali);
		bool init_Spec(const char* filepath_spec);
		bool validateSpecs(std::string);
		bool writeOutput(std::string);
	private:
		bool readInput(std::string);	
		bool transformValiInput();
		bool transformSpecInput();
		bool generateIndents();
		bool generateGlyphs();
		bool generateValues();
		bool generateSpecs();	
};

