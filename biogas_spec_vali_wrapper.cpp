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

#include "spec_vali_reader/biogas_spec_vali_reader.cpp"
#include "spec_vali_reader/biogas_spec_writer.cpp"
#include "spec_vali_reader/biogas_spec_validation.cpp"
#include "spec_vali_reader/biogas_spec_vali_reader.h"

static BiogasSpecValiReader* biogasReader;

extern "C" {
	
/**
 * Creates a new BiogasSpecValiReader object
 */
void readLUATableInit(){
	biogasReader = new BiogasSpecValiReader();
}

/**
 * Initialize the BiogasSpecValiReader
 * 
 * Method receives the absolute path to a validation or specification file
 * and an indicator "Spec" or "Vali" to call the corresponding
 * init function.
 * 
 * @param vali_or_spec: Chooses between validation files or specifications
 * @param filename: The absolute path to the file
 * @return Bool if the method was succesfull
 */
bool readLUATable(const char* filename, const char* vali_or_spec)
{
	if(((std::string) vali_or_spec) == "Vali")
		return biogasReader->init_Vali(filename);
	else if(((std::string) vali_or_spec) == "Spec")
		return biogasReader->init_Spec(filename);

	return false;
}

/**
 * Getter method for the validation String
 * 
 * This string contains all relevant data from the validation file in
 * a CSV style format. Each line corresponds to one tree item as they will
 * appear in LabView. The columns are as follows:
 * 
 * Col1: Indentations 
 * Col2: Glyphs
 * Col3: LeftCells
 * Col4: Types
 * Col5: Defaults
 *
 * @return All data of the validation file as String
 */
const char* getValiString()
{	
	return biogasReader->valiString.c_str();
}

/**
 * Getter method for the specification String
 * 
 * Values delimited by '\n'.
 *
 * @return String of values
 */
const char* getSpecString()
{	
	return biogasReader->specString.c_str();
}

/**
 * Getter method for the validation
 * 
 * @param specs: Current specification from the LabView Tree (string formatted)
 * @return Bool if specification is valid
 */
bool getValidation(const char* specs)
{
	return biogasReader->validateSpecs((std::string) specs);
}

/**
 * Getter method for the validation message
 * 
 * The validateSpecs() method needs to be called first.
 * 
 * @return Promt message for all errors in the validation
 */
const char* getValidationMessage()
{
	return biogasReader->validationMessage.c_str();
}

/**
 * Getter method for the validation Errors.
 * 
 * Values delimited by '\n'. The validateSpecs() method needs to be
 * called first.
 *
 * @return String of 'LeftCells' with failed validation
 */
const char* getValidationErrorParams()
{
	return biogasReader->validationErrorParams.c_str();
}

/**
 * Writes a specification file
 * 
 * Calls the writeOutput() method. If the input specs are valid 
 * a new specification file will be written.
 * 
 * @param specs: Current specification from the LabView Tree (string formatted)
 * @return Bool if the specs are valid
 */
bool getOutputSpecs(const char* specs)
{
	return biogasReader->writeOutputSpecs((std::string) specs);
}

/**
 * Getter method for the output specification
 * 
 * The getOutputSpecs() method needs to be called first.
 * 
 * @return The new specification file as string
 */
const char* getOutputString()
{
	return biogasReader->outputSpecs.c_str();
}

/**
 * Getter method for the number of parameters in the vali/spec file
 * 
 * @return The number of parameters
 */
int getNumberOfLines()
{
	return biogasReader->number_of_entries;
}

} //end extern "C" 
