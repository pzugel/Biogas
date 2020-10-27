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

/**
 * Write a new specification file
 *
 * Validates the given specification first. If they are valid
 * the method wirtes a new specification file into the
 * "outputSpecs" String.
 *
 * @param specs: Specifications committed by LabView
 * @return Bool whether the specs are valid
 */
bool BiogasSpecValiReader::
writeOutputSpecs(std::string specs)
{
	this->outputSpecs = "";
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
			this->outputSpecs += std::string(this->entries[i].indent, '\t');
			if(this->entries[i].glyph == 15)
				this->outputSpecs += this->entries[i].leftCell + "={\n";
			else
			{
				if (this->entries[i].leftCell.rfind("\"", 0) == 0)
					this->outputSpecs += "[" + this->entries[i].leftCell + "]=" + inputSpecs[i] + ",\n";
				else if(this->entries[i].leftCell == "timeTableContent")
					this->outputSpecs += inputSpecs[i] + ",\n";
				else
					this->outputSpecs += this->entries[i].leftCell + "=" + inputSpecs[i] + ",\n";
			}

			if(this->entries[i].indent>this->entries[i+1].indent)
			{
				int num_closing_par = (this->entries[i].indent)-(this->entries[i+1].indent);
				for(int j=0; j<num_closing_par; j++)
					this->outputSpecs += std::string(this->entries[i].indent-j-1,'\t') + "},\n";
			}
		}

		int num_closing_par_end = this->entries[this->number_of_entries-1].indent;
		for(int i=0; i<num_closing_par_end-1; i++)
			this->outputSpecs += std::string(num_closing_par_end-i-1,'\t') + "},\n";		
		this->outputSpecs += "}";
	}
	else
		return false;

	return true;
}

