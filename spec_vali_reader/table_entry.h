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

class TableEntry { 
	public:
		TableEntry(){};
		
		int indent = 0;
		int glyph = 0;
		std::string leftCell = "";
		std::string type = "";
		std::string defaultVal = ""; //Contains the default values
		std::string specVal = ""; //Contains Values from Spec file
		std::string rangeMin = "";
		std::string rangeMax = "";
};
