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

#include "output_reader/biogas_output_reader.cpp"

static BiogasOutputReader* biogasOutputReader;

extern "C" {

/**
 * Initialize the BiogasOutputReader
 * 
 * @param path_to_outputFiles: The absolute path to the outputFiles.lua
 * @return Bool if the method was succesfull
 */
bool readOutputFiles(const char* path_to_outputFiles)
{
	biogasOutputReader = new BiogasOutputReader();
	return biogasOutputReader->init(path_to_outputFiles);
}

/**
 * Getter method for the outputFilesTreeString String
 * 
 * This string contains data from an outputFiles.lua in a CSV style format. 
 * It is needed to construct the Plot-Tree structure in LabView.
 * The columns are as follows:
 *
 * Col1: LeftCells 
 * Col2: Indentations
 * Col3: Glyphs
 *
 * @return All data as String
 */
const char* getTreeString()
{
	return biogasOutputReader->outputFilesTreeString.c_str();
}

/**
 * Getter method for the outputFilesPlotString String
 * 
 * This string contains data from an outputFiles.lua in a CSV style format. 
 * It is needed to select specific columns from .txt output files and generate
 * plots with the correct labels. The columns are as follows:
 *
 * Col1: LeftCells (y Value)
 * Col2: Unit of leftCell
 * Col3: Column number (of this y Value in the .txt file)
 * Col4: Filename
 * Col5: Column number (of the x Value in the .txt file)
 * Col6: Unit of x Value
 * Col7: Name (x Value)
 *
 * @return All data as String
 */
const char* getPlotString()
{
	return biogasOutputReader->outputFilesPlotString.c_str();
}

/**
 * Getter method for the number of parameters in the output file
 * 
 * @return The number of parameters
 */
int getNumberOfOutputLines()
{
	return biogasOutputReader->number_of_lines_output;
}

} //end extern "C" 

