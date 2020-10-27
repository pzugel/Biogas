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

/**
 * Class to represents one row in the LabView Tree structure
 *
 * Contains all information given by a validation and specification
 * file concerning one parameter.
 *
 * @param indent: The indentation in the tree structure
 * @param glyph: Visual symbol in the tree strucutre (e.g. a folder symbol)
 * @param leftCell: Name of the parameter
 * @param type: Data type of the parameter (e.g. int, string)
 * @param defaultVal: Default value given by the validation file
 * @param specVal: Specification of the parameter
 * @param rangeMin: Optional range minimum (onyl for validation)
 * @param rangeMax: Optional range maximum (onyl for validation)
 */
class TableEntry { 
	public:
		TableEntry(){};
		
		int indent = 0;
		int glyph = 0;
		std::string leftCell = "";
		std::string type = "";
		std::string defaultVal = "";
		std::string specVal = "";
		std::string rangeMin = "";
		std::string rangeMax = "";
};
