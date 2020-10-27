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

/**
 * Validates specifications
 *
 * Verifies if given specificaions are the correct data
 * type and if they suit the range restrictions.
 * Parameters where the validation failed are added into the
 * "validationErrorParams" and a corresponding "validationMessage"
 * to display in LabView is generated.
 *
 * @param specs: Specifications committed by LabView
 * @return Bool whether the specs are valid
 */
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
	this->validationErrorParams = "";
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
				this->validationErrorParams += std::to_string(i) + "\n";
				isValid = false;
			}
		}

		if(this->entries[i].type == "Double")
		{
			if(!std::regex_match(inputSpecs[i], isDouble) &&
					!std::regex_match(inputSpecs[i], isDoubleTimestamp))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrorParams += std::to_string(i) + "\n";
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
					this->validationErrorParams += std::to_string(i) + "\n";
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
				this->validationErrorParams += std::to_string(i) + "\n";
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
					this->validationErrorParams += std::to_string(i) + "\n";
					isValid = false;
				}
			}
		}

		if(this->entries[i].type == "String")
		{
			if(!std::regex_match(inputSpecs[i], isString))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrorParams += std::to_string(i) + "\n";
				isValid = false;
			}
		}

		if(this->entries[i].type == "String[]")
		{
			if(!std::regex_match(inputSpecs[i], isStringArr))
			{
				this->validationMessage += "Type ERROR: \"" + this->entries[i].leftCell + "\" should be of type " +  this->entries[i].type + "\n";
				this->validationErrorParams += std::to_string(i) + "\n";
				isValid = false;
			}
		}
	}
	
	return isValid;
}
