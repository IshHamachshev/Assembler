#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include "globals.h"
#include "utils.h"
#include "label_table.h"

/*definition of the maximum size of a label*/
#define MAX_LABEL_SIZE 32

/*definition of decimal for strtol base*/
#define DECIMAL 10

/*first pass function*/
boolean firstPass(char* file_name, label_table_node* label_table_head, data_node** data_list_head, data_node** instruction_list_head, int* IC, int* DC);

/*Errors definition for the first pass*/
#define PRINT_ERR_LABEL_SIZE() fprintf(stderr, ERROR "The label %s in line %d is too long\n", label_name, line_number);
#define PRINT_ERR_LABEL_ALREADY_DEFINED(label_name, line_number) fprintf(stderr, ERROR "The label %s in line %d is already defined\n", label_name, line_number);
#define PRINT_ERR_MISSING_QUOTES_IN_STRING_DEFINITION(line_number) fprintf(stderr, ERROR "The string definition in line %d is missing quotes\n", line_number);
#define PRINT_ERR_MISSING_DATA_AFTER_INSTRUCTION(line_number) fprintf(stderr, ERROR "The data definition in line %d is missing\n", line_number);
#define PRINT_ERR_ILLEGAL_DATA_AFTER_INSTRUCTION(line_number) fprintf(stderr, ERROR "The data after the instruction in line %d isn't legal\n", line_number);
#define PRINT_ERR_ILLEGAL_LABEL_NAME(label_name, line_number) fprintf(stderr, ERROR "The label name %s in line %d isn't legal\n", label_name, line_number);
#define PRINT_ERR_INVALID_NUMBER_IN_DATA(line_number) fprintf(stderr, ERROR "There is an illegal number in line %d, each number has to be between -2048 to 2047\n", line_number);
#define PRINT_ERR_LABEL_BEFORE_EXTERN(line_number) fprintf(stderr, ERROR "Theres a label definition before .extern in line %d\n", line_number);
#define PRINT_ERR_MISSING_LABEL_AFTER_EXTERN(line_number) fprintf(stderr, ERROR "There's not label after .extern in line %d\n", line_number);
#define PRINT_ERR_ILLEGAL_INSTRUCTION_NAME(line_number) fprintf(stderr, ERROR "The instruction in line %d isn't an existing instruction\n", line_number);
#define PRINT_ERR_MISSING_OPERAND(line_number) fprintf(stderr, ERROR "There aren't enough operands in line %d\n", line_number);
#define PRINT_ERR_TEXT_AFTER_INSTRUCTION(line_number) fprintf(stderr, ERROR "There is text after all of the relevant information in line %d\n", line_number);
#define PRINT_ERR_INVALID_NUMBER_AFTER_INSTRUCTION(line_number) fprintf(stderr, ERROR "The number in line %d after the instruction is either too big or too small\n", line_number);

typedef enum {
    MOV,
    CMP,
    ADD,
    SUB,
    NOT,
    CLR,
    LEA,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    STOP
} OPCODE;

typedef enum{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
} REGISTER;

typedef enum{
    NO_TYPE = 0,
    IMMEDIATE_TYPE = 1,
    DIRECT_TYPE = 3,
    REGISTER_TYPE = 5
} OPERAND_TYPE;

typedef struct {
    int instruction_index;
    char* first_operand;
    OPERAND_TYPE first_operand_type;
    char* second_operand;
    OPERAND_TYPE second_operand_type;
} instruction_line;

typedef enum {
    SOURCE_OPERAND,
    DESINATION_OPERAND
}  OPERAND_ID;


#endif /*FIRSTPASS_H*/