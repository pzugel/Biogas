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

bool BiogasSpecValiReader::
init_Vali(const char* filepath_vali)
{
	bool isTransformed_Vali = false;
	bool isGenerated_Indent = false;
	bool isGenerated_Glyphs = false;
	bool isGenerated_Values = false;

	if(this->readInput((std::string) filepath_vali))
	{
		isTransformed_Vali = this->transformValiInput();
		isGenerated_Indent = this->generateIndents();
		isGenerated_Glyphs = this->generateGlyphs();
		isGenerated_Values = this->generateValues();
	}
	else	
		return false;

	return (isTransformed_Vali && isGenerated_Indent && isGenerated_Glyphs && isGenerated_Values);
}

bool BiogasSpecValiReader::
init_Spec(const char* filepath_spec)
{
	bool isTransformed_Spec = false;
	bool isGenerated_Specs = false;

	if(this->readInput((std::string) filepath_spec))
	{
		isTransformed_Spec = this->transformSpecInput();
		isGenerated_Specs = this->generateSpecs();
	}
	else
		return false;

	return (isTransformed_Spec && isGenerated_Specs);
}

bool BiogasSpecValiReader::
readInput(std::string filepath)
{
	this->input = "";

	std::ifstream LUATable(filepath);
	if(!LUATable.good())
	{
		return false;
	}

	for(std::string line; getline(LUATable,line);)
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
	return true;
}

bool BiogasSpecValiReader::
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
	return true;
}

bool BiogasSpecValiReader::
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
	return true;
}


bool BiogasSpecValiReader::
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
	return true;
}


bool BiogasSpecValiReader::
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
	return true;
}

bool BiogasSpecValiReader::
generateGlyphs()
{
	for(int i=0; i<this->number_of_entries-1; i++)
	{
		if(this->entries[i].indent<this->entries[i+1].indent)
		{
			this->entries[i].glyph = 15;
		}
	}
	return true;
}

bool BiogasSpecValiReader::
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

	return true;
}

bool BiogasSpecValiReader::
validateSpecs(std::string specs)
{
	std::vector<std::string> inputSpecs = {};
	std::istringstream lineIter(specs);
	for (std::string line; std::getline(lineIter, line); )
	{
		inputSpecs.push_back(line);
	}
	
	this->validationMessage = "";
	this->validationErrors = "";
	std::regex isBool ("true|false");
	std::regex isString ("\"[a-zA-Z0-9_.]+\"");
	std::regex isStringArr ("\\{\"[a-zA-Z0-9_]+\"\\}");
	std::regex isInt ("[0-9\\*]+");
	std::regex isDouble ("[0-9E.\\*\\-]+");
	std::regex isDoubleTimestamp ("\\{[0-9E.\\*\\-]+,[0-9E.\\*\\-]+\\}");
	std::regex isIntTimestamp ("\\{[0-9\\*]+,[0-9\\*]+\\}");

	bool isValid = true;
	for(int i=0; i<this->number_of_entries; i++)
	{
		if(this->entries[i].type == "Boolean")
		{
			if(!std::regex_match(inputSpecs[i], isBool))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrors += std::to_string(i) + "\n";
				isValid = false;
			}
		}

		if(this->entries[i].type == "Double")
		{
			if(!std::regex_match(inputSpecs[i], isDouble) &&
					!std::regex_match(inputSpecs[i], isDoubleTimestamp))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrors += std::to_string(i) + "\n";
				isValid = false;
			}
			if(!this->entries[i].rangeMin.empty() && !this->entries[i].rangeMax.empty())
			{ 
				if(std::regex_match(inputSpecs[i], isDouble) 
						&& (std::stod(inputSpecs[i])<std::stod(this->entries[i].rangeMin) || std::stod(inputSpecs[i])>std::stod(this->entries[i].rangeMax)))
				{
					this->validationMessage += "Range ERROR: "
						+ this->entries[i].leftCell
						+ " should be in Range {" 
						+ this->entries[i].rangeMin + "," 
						+ this->entries[i].rangeMax + "}\n";
					this->validationErrors += std::to_string(i) + "\n";
					isValid = false;
				}
			}
		}

		if(this->entries[i].type == "Integer")
		{
			if(!std::regex_match(inputSpecs[i], isInt) &&
					!std::regex_match(inputSpecs[i], isIntTimestamp))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrors += std::to_string(i) + "\n";
				isValid = false;
			}
			if(!this->entries[i].rangeMin.empty() && !this->entries[i].rangeMax.empty())
			{ 
				if(std::regex_match(inputSpecs[i], isInt) 
						&& (std::stoi(inputSpecs[i])<std::stoi(this->entries[i].rangeMin) || std::stoi(inputSpecs[i])>std::stoi(this->entries[i].rangeMax)))
				{
					this->validationMessage += "Range ERROR: "
						+ this->entries[i].leftCell
						+ " should be in Range {" 
						+ this->entries[i].rangeMin + "," 
						+ this->entries[i].rangeMax + "}\n";
					this->validationErrors += std::to_string(i) + "\n";
					isValid = false;
				}
			}
		}

		if(this->entries[i].type == "String")
		{
			if(!std::regex_match(inputSpecs[i], isString))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrors += std::to_string(i) + "\n";
				isValid = false;
			}
		}

		if(this->entries[i].type == "String[]")
		{
			if(!std::regex_match(inputSpecs[i], isStringArr))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrors += std::to_string(i) + "\n";
				isValid = false;
			}
		}
	}
	
	return isValid;
}


bool BiogasSpecValiReader::
writeOutput(std::string specs)
{
	outputString = "";
	if(validateSpecs(specs))
	{
		std::vector<std::string> inputSpecs = {};
		std::istringstream lineIter(specs);
		for (std::string line; std::getline(lineIter, line); )
		{
			inputSpecs.push_back(line);
		}
		for(int i=0; i<this->number_of_entries-1; i++)
		{
			outputString += std::string(this->entries[i].indent, '\t');
			if(this->entries[i].glyph == 15)
				outputString += this->entries[i].leftCell + "={\n";
			else
			{
				if (this->entries[i].leftCell.rfind("\"", 0) == 0)
					outputString += "[" + this->entries[i].leftCell + "]=" + inputSpecs[i] + ",\n";
				else if(this->entries[i].leftCell == "timeTableContent")
					outputString += inputSpecs[i] + ",\n";
				else
					outputString += this->entries[i].leftCell + "=" + inputSpecs[i] + ",\n";
			}

			if(this->entries[i].indent>this->entries[i+1].indent)
			{
				int num_closing_par = (this->entries[i].indent)-(this->entries[i+1].indent);
				for(int j=0; j<num_closing_par; j++)
					outputString += std::string(this->entries[i].indent-j-1,'\t') + "},\n";
			}
		}

		int num_closing_par_end = this->entries[this->number_of_entries-1].indent;
		for(int i=0; i<num_closing_par_end-1; i++)
			outputString += std::string(num_closing_par_end-i-1,'\t') + "},\n";		
		outputString += "}";
	}
	else
		return false;

	return true;
}

