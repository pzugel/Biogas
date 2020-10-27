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

#pragma once 
#include "table_entry.h"
#include <string>
#include <vector>

/**
 * Class to save all Data from a specification and validation file
 *
 * Following parameters are used to communicate with LabView:
 *
 * @param number_of_entries: Number of total parameters
 * @param valiString: All information of the validation file (CSV-style string)
 * @param specString: All information of the specification file (CSV-style string)
 * @param validationErrorParams: All names of parameters where the validation failed
 * @param validationMessage: Message to display in LabView
 * @param outputSpecs: String to write into specification file (after editing in LabView)
 *
 * Following parameters are internal:
 *
 * @param input: Input specification/validation file
 * @param input_valiModified: Modified validation input for easier parsing
 * @param input_specModified: Modified specification input for easier parsing
 * @param entries: Internal container for all vali/spec data
 */
class BiogasSpecValiReader { 
	public:
		int number_of_entries;

		std::string valiString;
		std::string specString;
		std::string validationErrorParams;
		std::string validationMessage;
		std::string outputSpecs;

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
		bool writeOutputSpecs(std::string);
	private:
		bool readInput(std::string);	
		void transformValiInput();
		void transformSpecInput();
		void generateIndents();
		void generateGlyphs();
		void generateValues();
		void generateSpecs();	
};

