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
#include <fstream>	
#include <regex>
#include <boost/algorithm/string.hpp>

/**
 * Transform the validation input
 *
 * Modifies the validation file it in such
 * a way that the "generateValues()" method can 
 * get easier access to the data.
 */
void BiogasSpecValiReader::
transformValiInput()
{
	std::regex range_value ("(range=\\{values=\\{[0-9\\.]+,[0-9\\.]+\\}\\})|(range=\\{[a-zA-Z0-9=\\.]+,[a-zA-Z0-9=\\.]+\\})");
	std::regex table_content ("tableContent=\\{values=\\{[\"a-zA-Z0-9_,]+\\}\\}");
	std::regex time_table_content ("timeTableContent=\\{numberEntries=[0-9]+\\}");
	std::regex table_element ("\"[a-zA-Z0-9_]+\"");
	std::regex number ("[0-9]+");
	std::regex character ("[a-zA-Z]+");
	this->input_valiModified = this->input;
	std::smatch match_range_value; //Replace all ranges with "range=[...]"
	while (regex_search(this->input_valiModified, match_range_value, range_value))
	{	
		std::string match = match_range_value.str(0);
		match = std::regex_replace(match, character, "");
		boost::replace_all(match, "{", "[");
		boost::replace_all(match, "}", "]");
		boost::replace_all(match, "=", "");
		boost::replace_all(match, "[[", "[");
		boost::replace_all(match, "]]", "]");
		boost::replace_all(match, ",", "-");
		std::string replacement = "range=" + match;
		boost::replace_all(this->input_valiModified, match_range_value.str(0), replacement);
	}

	std::smatch match_table; //Replace all tables with as many "{}" as there are elements in the table
	std::smatch match_table_entries;
	while (regex_search(this->input_valiModified, match_table, table_content)) 
	{
		std::string match = match_table.str(0);
		std::string replacement = "";
		while (regex_search(match, match_table_entries, table_element)) 
		{
			std::string tableEntry = match_table_entries.str(0);
			replacement += tableEntry + "={}";
			match = match_table_entries.suffix(); 
		}
		boost::replace_all(this->input_valiModified, match_table.str(0), replacement);
	} 

	std::smatch match_time_table; //Replace all time tables with as many "{}" as there are elements in the table
	std::smatch num_entries;
	while (regex_search(this->input_valiModified, match_time_table, time_table_content)) 
	{
		std::string match = match_time_table.str(0);
		std::string replacement = "";
		if(regex_search(match, num_entries, number))
		{
			int num = std::stoi(num_entries.str(0));
			for (int i=0; i<num+1;i++)
				replacement += "timeTableContent={}";
		}
		
		boost::replace_all(this->input_valiModified, match, replacement);  
	}
}

/**
 * Generate indentations
 *
 * Parse all paranthesis to assign the correct indentation.
 * This method also creates the TableEntry Objects. 
 * Therefor it has to be called first!
 */
void BiogasSpecValiReader::
generateIndents()
{
	int ind = -1;
	this->entries = {};

	for(int i=0; i<this->input_valiModified.size(); i++)
	{
		if(this->input_valiModified.at(i) == '{')
		{
			ind += 1;
			TableEntry* newEntry = new TableEntry();
			newEntry->indent = ind;
			this->entries.push_back(*newEntry);
		}
		else if(this->input_valiModified.at(i) == '}')
			ind -= 1;
	}

	this->number_of_entries = this->entries.size();
}

/**
 * Generate glyphs 
 *
 * Sets a visual symbol in LabView. If the entry is not a parameter, 
 * we set a folder symbol (15). Otherwise we keep the defaul (0).
 */
void BiogasSpecValiReader::
generateGlyphs()
{
	for(int i=0; i<this->number_of_entries-1; i++)
	{
		if(this->entries[i].indent<this->entries[i+1].indent)
		{
			this->entries[i].glyph = 15;
		}
	}
}

/**
 * Generate all validation data  
 *
 * Parses the validation file and searches for keywords
 * such as "type", "default" or "range" and their names. 
 * The values are then saved in the "entries"
 * data structure. The input string "input_valiModified" has 
 * been prepared by the "transformValiInput()" method for 
 * easier parsing.
 */
void BiogasSpecValiReader::
generateValues()
{
	std::string line_input = this->input_valiModified; 
	boost::replace_all(line_input, "{", "{\n");
	boost::replace_all(line_input, "}", "\n}\n");
	boost::replace_all(line_input, ",", "\n");

	std::regex names_re ("^[a-zA-Z0-9_\"]+=\\{");
	std::regex table_entry ("^((\")|(timeTableContent))");
	std::regex type_re ("^type=");
	std::regex default_re ("^default=");
	std::regex range_re ("^range=");
		
	int index = -1;
	std::istringstream lineIter(line_input);
	std::string last_type;
	std::string last_default;
	for (std::string line; std::getline(lineIter, line); )
	{
		if(std::regex_search(line, names_re))
		{
			++index;
			this->entries[index].leftCell = line.substr(0,line.size()-2);
			if(std::regex_search(line, table_entry))
			{
				this->entries[index].type = last_type;	
				this->entries[index].defaultVal = last_default;
			}
		}

		if(std::regex_search(line, type_re))
		{
			last_type = line.substr(6,line.size()-7);
			if(entries[index].glyph == 0)
				this->entries[index].type = last_type;
		}

		if(std::regex_search(line, default_re))
		{
			boost::replace_all(line, "\"", "");
			last_default = line.substr(8,line.size()-8);
			if(entries[index].glyph == 0)
				this->entries[index].defaultVal = last_default;
		}

		if(std::regex_search(line, range_re))
		{
			std::string::size_type min_pos = line.find("[");
			std::string::size_type max_pos = line.find("-");
			std::string::size_type end_pos = line.find("]");
			this->entries[index].rangeMin = line.substr(min_pos+1,max_pos-min_pos-1);
			this->entries[index].rangeMax = line.substr(max_pos+1,end_pos-max_pos-1);
		}
	}

	this->valiString = "";
	for(int i=0; i<this->number_of_entries; i++)
	{
		this->valiString += std::to_string(this->entries[i].indent) + " " + 
			std::to_string(this->entries[i].glyph) + " " + 
			this->entries[i].leftCell + " " +
			this->entries[i].type + " " +
			this->entries[i].defaultVal + "\n";		
	}
	this->valiString.resize(this->valiString.size() - 1);
}
