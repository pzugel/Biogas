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

#include "biogas_spec_vali_reader.h"
#include <string>
#include <vector>
#include <regex>
#include <boost/algorithm/string.hpp>

/**
 * Transform the specification input
 *
 * Modifies the specification file it in such
 * a way that the "generateSpecs()" method can 
 * get easier access to the data.
 */
void BiogasSpecValiReader::
transformSpecInput()
{
	this->input_specModified= this->input;

	this->input_specModified.erase(remove_if(this->input_specModified.begin(), this->input_specModified.end(), isspace), this->input_specModified.end());
	std::regex str_arr ("\\{\"[a-zA-Z0-9_]+\"\\}");
	std::regex timestamp ("\\{[0-9E.\\-\\*]+,[0-9E.\\-\\*]+\\}");

	std::smatch match_strArr; //Match types Str[]
	while (regex_search(this->input_specModified, match_strArr, str_arr)) 
	{
		std::string matched_str_arr = match_strArr.str(0).substr(1, match_strArr.str(0).length()-2);
		boost::replace_all(this->input_specModified, match_strArr.str(0), "$"+matched_str_arr+"?");  
	} 
	
	std::smatch match_timeStamps; //Match timestamps
	while (regex_search(this->input_specModified, match_timeStamps, timestamp)) 
	{
		std::string::size_type pos = match_timeStamps.str(0).find(",");
		std::string::size_type pos_end = match_timeStamps.str(0).find("}");
		std::string replacement = "$" + match_timeStamps.str(0).substr(1, pos-1) + "#" + match_timeStamps.str(0).substr(pos+1,pos_end-(pos+1)) + "?";
		boost::replace_all(this->input_specModified, match_timeStamps.str(0), replacement);  
	} 
	
	std::regex param_open ("\\{");
	std::regex param_close ("\\}$");
	std::regex param_close_comma ("\\},$");
	std::regex comma (",");
	std::regex eq ("=$");

	this->input_specModified = std::regex_replace(this->input_specModified, param_open, "\n");
	this->input_specModified = std::regex_replace(this->input_specModified, comma, "\n");

	std::string tmp = "";
	std::istringstream lineIter(this->input_specModified);
	for(std::string line; std::getline(lineIter, line); )
	{
		line = std::regex_replace(line, param_close_comma, "");
		line = std::regex_replace(line, param_close, "");
		line = std::regex_replace(line, comma, "");
		line = std::regex_replace(line, eq, "");

		if(!line.empty())
			tmp += line + "\n";
	}
	this->input_specModified = tmp;
	boost::replace_all(this->input_specModified, "$", "{");
	boost::replace_all(this->input_specModified, "?", "}");
	boost::replace_all(this->input_specModified, "#", ",");
}

/**
 * Generate all specification data  
 *
 * Parses the specification file and save all data in the "entries"
 * data structure. The input string "input_specModified" has been
 * prepared by the "transformSpecInput()" method for easier parsing.
 */
void BiogasSpecValiReader::
generateSpecs()
{	
	std::regex timestamp ("\\{[0-9E.\\-\\*]+,[0-9E.\\-\\*]+\\}");

	std::istringstream lineIter(this->input_specModified);
	int index = 0;
	for(std::string line; std::getline(lineIter, line); )
	{
		size_t valuePos = line.find("="); 
    	if (valuePos == std::string::npos)
		{
			if(std::regex_search(line, timestamp))
				this->entries[index].specVal = line;
		}
		else
        	this->entries[index].specVal = line.substr(valuePos+1);
		++index;
	}

	for(int i=0; i<this->number_of_entries; i++)
		this->specString += entries[i].specVal + "\n";
}


