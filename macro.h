#ifndef MACRO_H
#define MACRO_H

#include "globals.h"
#include "mtable.h"

/*a function that takes the name of the as file and makes a new file with the expanded macros*/
boolean expandMacros(const char* file_name);

#endif /* MACRO_H */