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
 * Class to represents one row in the LabView Plot-Tree structure
 *
 * Contains all information given by the outputFiles.lua
 * file concerning one parameter.
 *
 * @param indent: The indentation in the tree structure
 * @param glyph: Visual symbol in the tree strucutre (e.g. a folder symbol)
 * @param leftCell: Name of the parameter
 * @param filename: Name of the file where the parameter is given (*.txt)
 * @param unit: Unit of the parameter (e.g. [h])
 * @param column: Column of the parameter in the (CSV-style) textfile
 *
 * Each parameter is affiliated with an x value. 
 * @param xValueName: Name of the x value
 * @param xValueUnit: Unit of the x value
 * @param xValueColumn: Column of the x value in the (CSV-style) textfile
 */
class OutputEntry { 
	public:
		OutputEntry(){};
		
		int indent = 0;
		int glyph = 0;
		std::string leftCell = "";
		std::string filename = "";
		std::string unit = ""; 
		std::string column = "";

		std::string xValueName = "";
		std::string xValueUnit = ""; 
		std::string xValueColumn = "";
};


