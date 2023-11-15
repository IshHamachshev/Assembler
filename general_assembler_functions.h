#ifndef GENERAL_ASSEMBLER_FUNCTIONS_H
#define GENERAL_ASSEMBLER_FUNCTIONS_H

#include "globals.h"
#include "utils.h"
#include "label_table.h"

/*checks if there's a certain instruction*/
boolean isSpecificInstruction(const char* line, const char* instruction_type, boolean is_label);

/*functions overloading to isTheInstruction function for each instruction*/
boolean isData(const char* line, boolean is_label);
boolean isString(const char* line, boolean is_label);
boolean isEntry(const char* line, boolean is_label);
boolean isExtern(const char* line, boolean is_label);

/*checks if a line is a label and if label_name isn't NULL, allocates memory for label_name and copies the label name to there*/
boolean isLabel(const char* line, char** label_name);

#endif /*GENERAL_ASSEMBLER_FUNCTIONS_H*/