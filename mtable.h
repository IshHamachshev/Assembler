#ifndef MTABLE_H
#define MTABLE_H

#include "globals.h"

/*Struct of a node*/
typedef struct line_node{
    char* line;
    struct line_node* next;
} line_node;

/*Struct of the macro table entries*/
typedef struct {
    char* macro_name;
    line_node* lines_head;
} mtable_entry;

/*Struct of the macros table*/
typedef struct {
    int size;
    mtable_entry* entries;
} mtable;

/*Initializes a macro in the table*/
boolean initMacro(mtable* mtable, const char* macro_name);
/*create a mew mtable*/
mtable* createTable();
/*free a mtable*/
void freeTable(mtable* mtable);
/*add a value to the mtable*/
boolean addToTable(mtable* mtable , const char* macro_name , const char* line);
/*get the value from the mtable*/
line_node* getValue(mtable* mtable, const char* macro_name);

#endif /* MTABLE_H */