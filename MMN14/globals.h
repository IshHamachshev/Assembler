#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

/*Maximum length of a line including the character '\n' at the end*/
#define MAX_LINE_LENGTH 82

/*memory base address in decimal*/
#define MEMORY_BASE_ADDRESS 100

/*Definition of whitespaces*/
#define INLINE_WHITESPACES " \t"
#define WHITESPACES INLINE_WHITESPACES "\n"

/*Errors definitions*/
#define ERROR "Error: "
#define MACHINE_ERROR "Machine error: "
#define PRINT_ERR_MEMORY_ALLOCATION_FAILED fprintf(stderr, MACHINE_ERROR "Memory allocation failed\n");
#define PRINT_ERR_INVALID_INPUT fprintf(stderr, MACHINE_ERROR "Invalid input\n");
#define PRINT_ERR_OPEN_FILE_FAILED fprintf(stderr, MACHINE_ERROR "Opening a file failed\n");
#define PRINT_ERR_Close_FILE_FAILED fprintf(stderr, MACHINE_ERROR "Closing a file failed\n"); /*TODO: CHANGE FROM Close to CLOSE*/


/*Defining boolean*/
typedef enum {
    false = 0,
    true = 1
} boolean;

/*a definition of a word*/
typedef struct {
    int word:12;
} word;


/*a union for words and labels*/
typedef union{
    int word:12;
    char* label;
} data_union;

/*an enumaration for words and labels*/
typedef enum {
    WORD,
    LABEL
} data_enum;

/*a struct for data*/
typedef struct {
    data_union content;
    data_enum type;
} data;

/*a definition of a node for a list of words and labels(labels will be later changed for words)*/
typedef struct data_node{
    unsigned short address;
    data data;
    struct data_node* next;
} data_node ;



#define INSTRUCTION_LIST {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}
#define NUMBER_OF_INSTRUCTIONS 16
#endif /* GLOBALS_H */