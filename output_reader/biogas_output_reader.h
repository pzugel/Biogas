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

class BiogasOutputReader { 
	public:
		std::string plotTreeString;
		std::string plotValuesString;

		int number_of_lines_output;

	private:
		std::string input; //original input whithout linebreaks
		std::string input_modified; //Formatted original input with linebreaks

		std::vector<std::string> leftCellsVec;
		std::vector<std::string> filenamesVec;
		std::vector<std::string> colVec;
		std::vector<std::string> xcolVec;
		std::vector<std::string> unitVec;
		std::vector<std::string> xNameVec;
		std::vector<std::string> xUnitVec;
		std::vector<int> indentsVec;
		std::vector<int> glyphsVec;

	public:
		BiogasOutputReader(){};
		bool init(const char*);	

	private:
		bool load(std::string);
		bool readOutputFiles();
};

