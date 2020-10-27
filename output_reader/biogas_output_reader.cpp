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

#include "biogas_output_reader.h"
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <iostream>

/**
 * Initialize the BiogasOutputReader
 *	
 * Executes all methods to read in the outputFiles.lua and construct the
 * "outputFilesTreeString" and "outputFilesPlotString" to communicate with
 * LabView. 
 * 
 * @param output_path: The absolute path to the outputFiles.lua
 * @return Bool if the method was succesfull
 */
bool BiogasOutputReader::
init(const char* output_path)
{
	if(this->load((std::string) output_path))
	{
		this->readOutputFiles();
		this->generateTreeString();
		this->generatePlotString();
		return true;
	}
	else	
		return false;

	return true;
}

/**
 * Load the outputFiles.lua
 *	
 * Read in the outputFiles.lua and remove all comments and
 * formattings (whitespaces, linebreaks, tabs). The "input"
 * string now is a continous text.
 * 
 * @param filepath: The absolute path to the outputFiles.lua
 * @return Bool if file could be read
 */
bool BiogasOutputReader::
load(std::string filepath)
{
	this->input = "";
	
	std::ifstream LUAOutput(filepath);
	if(!LUAOutput.good())
	{
		return false;
	}

	for(std::string line; getline(LUAOutput,line);)
	{
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
		std::string prefix("--");
		if (line.compare(0, prefix.size(), prefix))
		{
			if(!line.empty())
			{
				if (line.find("--") != std::string::npos)
				{
					this->input += line.substr(0, line.find("--"));
				}
				else
				{
					this->input += line;
				}		
			}
		}	
	}

	this->input.erase(remove_if(this->input.begin(), this->input.end(), isspace), this->input.end());
	return true;
}

/**
 * Filter all x Values 
 *	
 * Every parameter from the outputFiles.lua needs to
 * be affiliated to a specific x Value. This method
 * reads in all data from the x Values and saves them 
 * in vectors.
 */
void BiogasOutputReader:: 
readXValues(std::vector<std::string> *x_Names, std::vector<std::string> *x_Units, std::vector<std::string> *x_Cols)
{
	std::regex x_value ("x=\\{[a-zA-Z0-9_]+=\\{unit=\"[a-zA-Z0-9\\^\\[\\]]+\",col=[0-9]+");
	std::string tmp_string = this->input;
	std::smatch match_x_values;
	while (regex_search(tmp_string, match_x_values, x_value)) { 
		std::string match = match_x_values.str(0);
		boost::replace_all(match, "{", "");

		std::string::size_type col_pos = match.find(",col=");
		x_Cols->push_back(match.substr(col_pos+5));

		std::string::size_type unit_pos = match.find("unit=");
		x_Units->push_back(match.substr(unit_pos+5, col_pos-unit_pos-5));

		std::string::size_type name_pos = match.find("x=");
		x_Names->push_back(match.substr(name_pos+2, unit_pos-3));

        tmp_string = match_x_values.suffix().str(); 
    } 
}

/**
 * Modify the Input
 *	
 * Inserts linebreaks and removes unwanted lines from the 
 * outputFiles.lua, e.g. the x Values are already read 
 * in the "readXValues" method so they can be removed.
 * "input" was a continous string, "input_modified" can 
 * now be read line by line.
 */
void BiogasOutputReader:: 
modifyInput()
{
	this->input_modified = this->input;

	std::regex x_value ("x=\\{[a-zA-Z0-9_]+=\\{unit=\"[a-zA-Z0-9\\^\\[\\]]+\",col=[0-9]+");
	std::regex keys ("keys=\\{");
	std::regex outputFiles ("outputFiles=\\{");

	this->input_modified = std::regex_replace(this->input_modified, x_value, "");
	this->input_modified = std::regex_replace(this->input_modified, keys, "");
	this->input_modified = std::regex_replace(this->input_modified, outputFiles, "");

	boost::replace_all(this->input_modified, "{", "{\n");
	boost::replace_all(this->input_modified, ",", ",\n");
}

/**
 * Generate date from the outputFiles.lua
 *	
 * Constructs a vector "entries" of type "OutputEntry"
 * and fills in the data from the outputFiles.lua
 * 
 * @return Bool if successful
 */
bool BiogasOutputReader::
readOutputFiles()
{	
	this->entries = {};
	
	std::vector<std::string> x_Names = {};
	std::vector<std::string> x_Units = {};
	std::vector<std::string> x_Cols = {};
	this->readXValues(&x_Names, &x_Units, &x_Cols);

	std::regex param ("^[a-zA-Z0-9_]+=");
	std::regex filename ("^filename=");
	std::regex col ("^col=");
	std::regex unit ("^unit=");
	std::regex names ("^[a-zA-Z0-9_]+=\\{");
	std::regex y_value ("^y=\\{");

	this->modifyInput();
	std::istringstream lineIter(this->input_modified);
	std::string lastFileName = "";
	std::string lastXCol = "";
	std::string lastXName = "";
	std::string lastXUnit = "";

	int ind = -1;
	for(std::string line; std::getline(lineIter, line); )
	{	
		if(std::regex_search(line, param))
		{
			if(std::regex_search(line, names))
			{
				if(std::regex_search(line, y_value))
				{
					this->entries[ind].indent = 0;
					this->entries[ind].glyph = 15;
					this->entries[ind].leftCell = this->entries[ind].leftCell;
				}
				else
				{
					ind += 1;
					// Contruct new Element
					OutputEntry* newEntry = new OutputEntry();
					this->entries.push_back(*newEntry);

					this->entries[ind].indent = 1;
					this->entries[ind].glyph = 37;
					boost::replace_all(line, "={", "");
					this->entries[ind].leftCell = line;
					this->entries[ind].filename = lastFileName;
				}
			} 
			else if(std::regex_search(line, filename))	
			{
				boost::replace_all(line, ",", "");
				boost::replace_all(line, "\"", "");
				boost::replace_all(line, "filename=", "");
				lastFileName = line;
				this->entries[ind].filename = line;
				this->entries[ind].column = "";
				this->entries[ind].unit = "";	
				
				lastXCol = std::to_string(std::stoi(x_Cols.front())-1);
				this->entries[ind].xValueColumn = lastXCol;
				x_Cols.erase(x_Cols.begin());
				
				lastXUnit = x_Units.front();
				boost::replace_all(lastXUnit, "\"", "");
				this->entries[ind].xValueUnit = lastXUnit;
				x_Units.erase(x_Units.begin());
				
				lastXName = x_Names.front();
				this->entries[ind].xValueName = lastXName;
				x_Names.erase(x_Names.begin());
			}
			else if(std::regex_search(line, col))	
			{
				boost::replace_all(line, "col=", "");
				boost::replace_all(line, "}", "");
				boost::replace_all(line, ",", "");
				this->entries[ind].column = std::to_string(std::stoi(line)-1);
				this->entries[ind].xValueColumn = lastXCol;
				this->entries[ind].xValueName = lastXName;
				this->entries[ind].xValueUnit = lastXUnit;
			}
			else if(std::regex_search(line, unit))	
			{
				boost::replace_all(line, "unit=", "");
				boost::replace_all(line, "}", "");
				boost::replace_all(line, ",", "");
				boost::replace_all(line, "\"", "");
				this->entries[ind].unit = line;
			}			
		}	
	}

	this->number_of_lines_output = this->entries.size();
	return true;
}

/**
 * Write the "outputFilesTreeString" from the generated "entries"
 */
void BiogasOutputReader::
generateTreeString()
{	
	this->outputFilesTreeString = "";
	for(int i=0; i<this->number_of_lines_output; i++)
	{
		this->outputFilesTreeString += this->entries[i].leftCell + " " + 
			std::to_string(this->entries[i].indent) + " " + 
			std::to_string(this->entries[i].glyph) + "\n";		
	}
	this->outputFilesTreeString.resize(this->outputFilesTreeString.size() - 1);
}

/**
 * Write the "outputFilesPlotString" from the generated "entries"
 */
void BiogasOutputReader::
generatePlotString()
{	
	this->outputFilesPlotString = "";
	for(int i=0; i<this->number_of_lines_output; i++)
	{
		this->outputFilesPlotString += this->entries[i].leftCell +  " " + this->entries[i].unit + " " + 
			this->entries[i].column + " " + 
			this->entries[i].filename + " " +		
			this->entries[i].xValueColumn + " " +
			this->entries[i].xValueName + " " + this->entries[i].xValueUnit + "\n";
	}
	this->outputFilesPlotString.resize(this->outputFilesPlotString.size() - 1);
}
