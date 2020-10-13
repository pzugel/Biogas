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

bool BiogasOutputReader::
init(const char* output_path)
{
	if(this->load((std::string) output_path))
	{
		this->readOutputFiles();
		return true;
	}
	else	
		return false;

	return true;
}

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

bool BiogasOutputReader::
readOutputFiles()
{
	leftCellsVec = {};
	indentsVec = {};
	filenamesVec = {};
	xNameVec = {};
	glyphsVec = {};
	xcolVec = {};

	this->input_modified = this->input;

	std::regex x_value ("x=\\{[a-zA-Z0-9_]+=\\{unit=\"[a-zA-Z0-9\\^\\[\\]]+\",col=[0-9]+");
	std::regex keys ("keys=\\{");
	std::regex outputFiles ("outputFiles=\\{");

	std::smatch match_x_values;
	std::vector<std::string> x_Names = {};
	std::vector<std::string> x_Units = {};
	std::vector<std::string> x_Cols = {};

	std::string tmp_string = this->input;
	while (regex_search(tmp_string, match_x_values, x_value)) { 
		std::string match = match_x_values.str(0);
		boost::replace_all(match, "{", "");

		std::string::size_type col_pos = match.find(",col=");
		x_Cols.push_back(match.substr(col_pos+5));

		std::string::size_type unit_pos = match.find("unit=");
		x_Units.push_back(match.substr(unit_pos+5, col_pos-unit_pos-5));

		std::string::size_type name_pos = match.find("x=");
		x_Names.push_back(match.substr(name_pos+2, unit_pos-3));

        tmp_string = match_x_values.suffix().str(); 
    } 

	this->input_modified = std::regex_replace(this->input_modified, x_value, "");
	this->input_modified = std::regex_replace(this->input_modified, keys, "");
	this->input_modified = std::regex_replace(this->input_modified, outputFiles, "");

	boost::replace_all(this->input_modified, "{", "{\n");
	boost::replace_all(this->input_modified, ",", ",\n");

	std::regex param ("^[a-zA-Z0-9_]+=");
	std::regex filename ("^filename=");
	std::regex col ("^col=");
	std::regex unit ("^unit=");
	std::regex names ("^[a-zA-Z0-9_]+=\\{");
	std::regex y_value ("^y=\\{");

	std::istringstream lineIter(this->input_modified);
	std::string lastFileName = "";
	std::string lastXCol = "";
	std::string lastXName = "";
	std::string lastXUnit = "";

	for(std::string line; std::getline(lineIter, line); )
	{	
		if(std::regex_search(line, param))
		{
			if(std::regex_search(line, names))
			{
				if(std::regex_search(line, y_value))
				{
					this->indentsVec.pop_back();
					this->indentsVec.push_back(0);

					this->glyphsVec.pop_back();
					this->glyphsVec.push_back(15);

					std::string tmp = this->leftCellsVec.back();
					this->leftCellsVec.pop_back();
					this->leftCellsVec.push_back(tmp);
				}
				else
				{
					this->indentsVec.push_back(1);
					this->glyphsVec.push_back(37);
					boost::replace_all(line, "={", "");
					this->leftCellsVec.push_back(line);
					this->filenamesVec.push_back(lastFileName);
				}
			} 
			else if(std::regex_search(line, filename))	
			{
				this->filenamesVec.pop_back();
				boost::replace_all(line, ",", "");
				boost::replace_all(line, "\"", "");
				boost::replace_all(line, "filename=", "");
				lastFileName = line;
				this->filenamesVec.push_back(line);
				this->colVec.push_back("");
				this->unitVec.push_back("");

				lastXCol = std::to_string(std::stoi(x_Cols.front())-1);
				this->xcolVec.push_back(lastXCol);
				x_Cols.erase(x_Cols.begin());
				
				lastXUnit = x_Units.front();
				boost::replace_all(lastXUnit, "\"", "");
				this->xUnitVec.push_back(lastXUnit);
				x_Units.erase(x_Units.begin());
				
				lastXName = x_Names.front();
				this->xNameVec.push_back(lastXName);
				x_Names.erase(x_Names.begin());
			}
			else if(std::regex_search(line, col))	
			{
				boost::replace_all(line, "col=", "");
				boost::replace_all(line, "}", "");
				boost::replace_all(line, ",", "");
				this->colVec.push_back(std::to_string(std::stoi(line)-1));
				this->xcolVec.push_back(lastXCol);
				this->xNameVec.push_back(lastXName);
				this->xUnitVec.push_back(lastXUnit);
			}
			else if(std::regex_search(line, unit))	
			{
				boost::replace_all(line, "unit=", "");
				boost::replace_all(line, "}", "");
				boost::replace_all(line, ",", "");
				boost::replace_all(line, "\"", "");
				this->unitVec.push_back(line);
			}	
		}	
	}
	this->number_of_lines_output = this->leftCellsVec.size();

	/**
	 *	Fill "plotTreeString" and "plotValuesString" to communicate with LabVIew
	 */

	this->plotTreeString = "";
	for(int i=0; i<this->number_of_lines_output; i++)
	{
		this->plotTreeString += this->leftCellsVec[i] + " " + 
			std::to_string(this->indentsVec[i]) + " " + 
			std::to_string(this->glyphsVec[i]) + "\n";		
	}
	this->plotTreeString.resize(this->plotTreeString.size() - 1);

	this->plotValuesString = "";
	for(int i=0; i<this->number_of_lines_output; i++)
	{
		this->plotValuesString += this->leftCellsVec[i] +  " " + this->unitVec[i] + " " + 
			this->colVec[i] + " " + 
			this->filenamesVec[i] + " " +		
			this->xcolVec[i] + " " +
			this->xNameVec[i] + " " + this->xUnitVec[i] + "\n";
	}
	this->plotValuesString.resize(this->plotValuesString.size() - 1);
	
	return true;
}
