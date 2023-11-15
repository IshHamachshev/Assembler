#include "general_assembler_functions.h"
#include "firstpass.h"
#include <errno.h>
#include <ctype.h>

#define MINIMUM_12BITS_SIGNED -2048
#define MAXIMUM_12BITS_SIGNED 2047

#define FOUND_ERROR first_pass_found_error = true;


/*this function returns the registerID (enum) from its name(string)*/
REGISTER getRegister(char* token);

/*this function checks if a string is in a register format*/
boolean isRegister(char* token);

/*this function gets the operandType from its string(token in orginal line) and returns error if is invalid*/
OPERAND_TYPE getOperandType(char* operand, int line_number);

/*this function checks the syntax of a string and validate it's in a valid label format*/
boolean isLegalLabelName(char* label_name);

/*this function updates the linked list that holds the instructions,data,and labels*/
/*on the firstpass it holds the instructions word encoded, the data encoded, and the label names (it will be replaced on the second pass with the lanbel address) */
void addWordOrLabelToList(data_node** head, data data, unsigned short address);

char* getString(char* line);
char* getData(char* line);

/*this function adds all the labels after .extern from the input line to the labels_table and set the extern flag in the table to true*/
boolean addExternToTable(char* line, label_table_node* label_table_head);

/*this function extracts the instruction name (3 chars name) from the original line and returns the instruction opcoed ID (enum)*/
/* it returns -1 if there isn't a valid and known instruction */
int getInstructionIndex(char* line, boolean is_label);


/* this function  gets the parsed instruction line in a instruction_line structs and adds to the instructions linked list all the words of the firstpass for this instruction line*/
/* first the instruction word with the operands types*/
/* second the values,adresses of the instrucion operands*/
/* on this oreder :*/
/*  if the source operand is label or direct value - insert the label name (it will ber replaced in  the second pass)*/
/*  if not and there are registers write the word of registrers reference*/
/* if the second operand is label or direct value - insert the label name (it wiil ber replaced in  the second pass)*/ 
/*the function returns L for IC <- IC+L, and returns -1 if there an error occured*/
int addInstructionLineToList(instruction_line* instruction_line, data_node** data_list_head, int IC);


int getOperandsCount(int instructionID);
boolean isValidOperand(int instruction_ID, int operand_ID, int operand_type);
boolean parseInstruction(char* line, boolean is_label, instruction_line* instruction_line, int line_number);

REGISTER getRegister(char* token){
    if(token == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    if(strncmp(token, "@r0", 3) == 0) {
        return R0;
    }
    if(strncmp(token, "@r1", 3) == 0) {
        return R1;
    }
    if(strncmp(token, "@r2", 3) == 0) {
        return R2;
    }
    if(strncmp(token, "@r3", 3) == 0) {
        return R3;
    }
    if(strncmp(token, "@r4", 3) == 0) {
        return R4;
    }
    if(strncmp(token, "@r5", 3) == 0) {
        return R5;
    }
    if(strncmp(token, "@r6", 3) == 0) {
        return R6;
    }
    return R7;
}

/*checks if a string is a register*/
boolean isRegister(char* token){
    if(token == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
    if(strlen(token) != 3){/*checks if the length is invalid*/
        return false;
    }
    if(token[0] != '@' || token[1] != 'r' || token[2] > '7' || token[2] < '0'){/*checks if the register isn't legal*/
        return false;
    }
    return true;
}

OPERAND_TYPE getOperandType(char* operand, int line_number){
    long temp_number = 0;
    char* temp_endptr = NULL;
    if(isRegister(operand)){
        return REGISTER_TYPE;
    }
    temp_number = strtol(operand, &temp_endptr, DECIMAL);
    if(errno == 0 && operand != temp_endptr && *temp_endptr == '\0'){
        if(temp_number < MINIMUM_12BITS_SIGNED || temp_number > MAXIMUM_12BITS_SIGNED){
            PRINT_ERR_INVALID_NUMBER_AFTER_INSTRUCTION(line_number)
            return NO_TYPE;
        }
        return IMMEDIATE_TYPE; 
    }
    if(isLegalLabelName(operand)){
        return DIRECT_TYPE;
    }
    else{
        PRINT_ERR_ILLEGAL_LABEL_NAME(operand, line_number)
        return NO_TYPE;
    }
    
}

/*the function checks if the label name is legal*/
boolean isLegalLabelName(char* label_name){
    int i = 0;
    if(label_name == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    if(strlen(label_name) >= MAX_LABEL_SIZE || strlen(label_name) == 0){/*checks if the length of the label is legal*/
        return false;
    }
    if(!isalpha(label_name[0])){/*checks if the first letter of the label isn't alphabetical*/
        return false;
    }
    for(i=1; i < strlen(label_name); i++){
        if(!isalnum(label_name[i])){/*checks any character in the label isn't alphanumeric*/
            return false;
        }
    }
    return true;
}

/*the function adds a word to a list*/
void addWordOrLabelToList(data_node** head, data data, unsigned short address){
    data_node* node = NULL;
    if(head == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    if(*head == NULL){/*if the linked list wasnt initalized*/
        *head = (data_node*)(malloc(sizeof(data_node)));
        if(*head == NULL){
            PRINT_ERR_MEMORY_ALLOCATION_FAILED
            exit(1);
        }
        (*head)->address = address;
        (*head)->next = NULL;
        if(data.type == WORD){
            (*head)->data.content.word = data.content.word;
            (*head)->data.type = data.type;
        }
        else{
            /* insert to the linked list the labels name  - it will be replaced with the address on the second pass*/
            (*head)->data.content.label = strdup(data.content.label);
            if((*head)->data.content.label == NULL){
                PRINT_ERR_MEMORY_ALLOCATION_FAILED
                exit(1);
            }
            (*head)->data.type = data.type;
        }
    }
    else{
        node = *head;
        while(node->next != NULL){
            node = node->next;
        }
        node->next = (data_node*)(malloc(sizeof(data_node)));
        if(node->next == NULL){
            PRINT_ERR_MEMORY_ALLOCATION_FAILED
            exit(1);
        }
        node->next->address = address;
        node->next->next = NULL;
        if (data.type == WORD) {
            node->next->data.content.word = data.content.word;
            node->next->data.type = data.type;
        }
        else if (data.type == LABEL) {
            /* insert to the linked list the labels name  - it will be replaced with the address on the second pass*/
            node->next->data.content.label = strdup(data.content.label);
            if(node->next->data.content.label == NULL){
                PRINT_ERR_MEMORY_ALLOCATION_FAILED
                exit(1);
            }
            node->next->data.type = data.type;
        }
    }
}

char* getString(char* line){
    char* token = NULL;
    char* line_duplicate = strdup(line);
    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }

    token = strtok(line_duplicate, "\"");
    if(token == NULL){
        free(line_duplicate);
        return NULL;
    }
    token = strtok(NULL, "\"");
    if(token == NULL){
        free(line_duplicate);
        return NULL;
    }
    
    token = strdup(token);
    if(token == NULL){
        free(line_duplicate);
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    free(line_duplicate);
    return token;
}

char* getData(char* line){
    char* token = NULL;
    char* line_duplicate = strdup(line);
    if(line_duplicate[0] != '.'){
        token = strtok(line_duplicate, ".");
        if(token == NULL){
            free(line_duplicate);
            return NULL;
        }
        token = strtok(NULL, WHITESPACES);
        if(token == NULL){
            free(line_duplicate);
            return NULL;
        }
    }
    else{
        token = strtok(line_duplicate, WHITESPACES);
        if(token == NULL){
            free(line_duplicate);
            return NULL;
        }
    }

    token = strtok(NULL, "\n");
    if(token == NULL){
        free(line_duplicate);
        return NULL;
    }

    token = strdup(token);
    if(token == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    free(line_duplicate);
    return token;
}
/*the function adds all the labels after .extern to the label_table*/
boolean addExternToTable(char* line, label_table_node* label_table_head){
    char* token = NULL;
    char* line_duplicate = strdup(line); /*duplicating the line*/

    token = strtok(line_duplicate, WHITESPACES);
    if(token == NULL){/*checks if the .extern line isn't legal*/
        free(line_duplicate);
        return false;
    }   
    token = strtok(NULL, WHITESPACES);
    if(token == NULL){/*checks if there's any token after .extern, if there's no token the line is illegal*/
        free(line_duplicate);
        return false;
    }

    while(token != NULL){/*loops over all the words in the line after .extern and adds them to the table*/
        addToLabelTable(label_table_head, strdup(token), 0, EXTERN); /*adds the token to the label table as an extern without an address*/
        token = strtok(NULL, WHITESPACES);
    }
    return true;
    free(line_duplicate);
}

/*the function checks if an instruction is in the instructions list*/
int getInstructionIndex(char* line, boolean is_label){
    int i=0;
    char* token = NULL;
    char* line_duplicate = strdup(line);
    const char* instructions[] = INSTRUCTION_LIST;
    
   if(is_label){ /*if the line is a label, the instruction is the second token*/
    token = strtok(line_duplicate, ":" WHITESPACES);
    if(token == NULL){ 
        free(line_duplicate);
        return -1;
    }
    token = strtok(NULL, WHITESPACES);
   }
    else{/*if there's no label, the instruction is the first token in the line*/
    token = strtok(line_duplicate, WHITESPACES);
   }
   if(token == NULL){ /*makes sure the token is valid*/
        free(line_duplicate);
        return -1;
    }

    for(i=0; i < NUMBER_OF_INSTRUCTIONS; i++){ /*loops over all the instruction names and checks if it is any of them,*/
        if(strcmp(token, instructions[i]) == 0){
            token = strdup(token); /*allocating the instruction*/
            return i;
        }
    }
    free(line_duplicate);
    return -1;
}


/* this function  gets the parsed instruction line in a instruction_line structs and adds to the instructions linked list all the words of the firstpass for this instruction line*/
/* first the instruction word with the operands types*/
/* second the values,adresses of the instrucion operands*/
/* on this oreder :*/
/*  if the source operand is label or direct value - insert the label name (it wiil ber replaced in  the second pass)*/
/*  if not and there are registers write the word of registrers reference*/
/* if the second operand is label or direct value - insert the label name (it wiil ber replaced in  the second pass)*/ 
/*the function returns L for IC <- IC+L, and returns -1 if there an error occured*/
int addInstructionLineToList(instruction_line* instruction_line, data_node** data_list_head, int IC){
    data temp_data = {0};
    int temp_word = 0;
    int L = 0;
    temp_data.type = WORD;
    temp_data.content.word = (instruction_line->first_operand_type << 9);
    temp_data.content.word += (instruction_line->instruction_index << 5);
    temp_data.content.word += (instruction_line->second_operand_type << 2);
    addWordOrLabelToList(data_list_head, temp_data, IC);
    L++;
    if(instruction_line->first_operand_type == DIRECT_TYPE){
        temp_data.content.label = instruction_line->first_operand;
        temp_data.type = LABEL;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->first_operand_type == IMMEDIATE_TYPE){
        temp_data.content.word = (int)((atol(instruction_line->first_operand)) << 2);
        temp_data.type = WORD;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->first_operand_type == REGISTER_TYPE && instruction_line->second_operand_type == REGISTER_TYPE){
        temp_word = 0;
        temp_word += (getRegister(instruction_line->first_operand) << 7);
        temp_word += (getRegister(instruction_line->second_operand) << 2);
        temp_data.content.word = temp_word;
        temp_data.type = WORD;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->first_operand_type != REGISTER_TYPE && instruction_line->second_operand_type == REGISTER_TYPE){
        temp_word = 0;
        temp_word += ( getRegister(instruction_line->second_operand) << 2);
        temp_data.content.word = temp_word;
        temp_data.type = WORD;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->first_operand_type == REGISTER_TYPE && instruction_line->second_operand_type != REGISTER_TYPE){
        temp_word = 0;
        temp_word += (getRegister(instruction_line->first_operand) << 7);
        temp_data.content.word = temp_word;
        temp_data.type = WORD;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->second_operand_type == DIRECT_TYPE){
        temp_data.content.label = instruction_line->second_operand;
        temp_data.type = LABEL;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    if(instruction_line->second_operand_type == IMMEDIATE_TYPE){
        temp_data.content.word = (int)((atol(instruction_line->second_operand)) << 2);
        temp_data.type = WORD;
        addWordOrLabelToList(data_list_head, temp_data, IC + L);
        L++;
    }
    return L;
}

int getOperandsCount(int instructionID){
	if (instructionID == MOV || instructionID == CMP  || instructionID == ADD || instructionID == SUB || instructionID == LEA){
		return 2;
	}
	if (instructionID == RTS || instructionID == STOP){
		return 0;
	}
	return 1;
}

boolean isValidOperand(int instruction_ID, int operand_ID, int operand_type){
    if(operand_ID == SOURCE_OPERAND){
        if(operand_type != NO_TYPE && (instruction_ID == MOV || instruction_ID == CMP || instruction_ID == ADD  || instruction_ID == SUB )){
            return true;
        }
        else if(operand_ID == DIRECT_TYPE && instruction_ID == LEA){
            return true;
        }
    }
    if(operand_ID == DESINATION_OPERAND){
        if((operand_type == DIRECT_TYPE || operand_type == REGISTER_TYPE) && (instruction_ID != RTS && instruction_ID != STOP)){
            return true;
        }
        else if(operand_type == IMMEDIATE_TYPE && (instruction_ID == CMP || instruction_ID == PRN)){
            return true;
        }
    }
    return false;
}

boolean parseInstruction(char* line, boolean is_label, instruction_line* instruction_line, int line_number){
    char* line_duplicate = strdup(line);
    char* instruction_token = NULL;
    char* first_operand_token = NULL;
    char* second_operand_token = NULL;
    int instruction_index = 0;

    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    if(is_label){
        instruction_token = strtok(line_duplicate, ":" WHITESPACES);
        if(instruction_token == NULL){
            free(line_duplicate);
            return false;
        }
        instruction_token = strtok(NULL, WHITESPACES);
        if(instruction_token == NULL){
            free(line_duplicate);
            return false;
        }
    }
    else{
        instruction_token = strtok(line_duplicate, WHITESPACES);
        if(instruction_token == NULL){
            free(line_duplicate);
            return false;
        }
    }
    instruction_index = getInstructionIndex(line, is_label);

    if(instruction_index == -1){
        PRINT_ERR_ILLEGAL_INSTRUCTION_NAME(line_number)
        free(line_duplicate);
        return false;
    }
    if(getOperandsCount(instruction_index) == 0){/*there shouldn't be any operands*/
        if (strtok(NULL, "," WHITESPACES) != NULL){
            PRINT_ERR_TEXT_AFTER_INSTRUCTION(line_number)
            free(line_duplicate);
            return false;
        }
        instruction_line->instruction_index = instruction_index;
        instruction_line->first_operand = NULL;
        instruction_line->second_operand = NULL;
        instruction_line->first_operand_type = NO_TYPE;
        instruction_line->second_operand_type = NO_TYPE;
    }
    else if(getOperandsCount(instruction_index) == 1){/*there should be exactly one operand*/
        first_operand_token = strtok(NULL, "," WHITESPACES);
        if(first_operand_token == NULL){
            PRINT_ERR_MISSING_DATA_AFTER_INSTRUCTION(line_number)
            return false;
        }
        if (strtok(NULL, WHITESPACES) != NULL){
            PRINT_ERR_TEXT_AFTER_INSTRUCTION(line_number)
            free(line_duplicate);
            return false;
        }
        instruction_line->instruction_index = instruction_index;
        instruction_line->first_operand = NULL;
        instruction_line->second_operand = strdup(first_operand_token);
        instruction_line->first_operand_type = NO_TYPE;
        instruction_line->second_operand_type = getOperandType(first_operand_token, line_number);
    }
    else { /*there should be exactly 2 operands*/
        first_operand_token = strtok(NULL, "," WHITESPACES);
        if(first_operand_token == NULL){
            PRINT_ERR_MISSING_DATA_AFTER_INSTRUCTION(line_number)
            return false;
        }
        second_operand_token = strtok(NULL, "," WHITESPACES);
        if(second_operand_token == NULL){
            PRINT_ERR_MISSING_DATA_AFTER_INSTRUCTION(line_number)
            return false;
        }
        if (strtok(NULL, WHITESPACES) != NULL){
            PRINT_ERR_TEXT_AFTER_INSTRUCTION(line_number)
            free(line_duplicate);
            return false;
        }
        instruction_line->instruction_index = instruction_index;
        instruction_line->first_operand = strdup(first_operand_token);
        instruction_line->second_operand = strdup(second_operand_token);
        instruction_line->first_operand_type = getOperandType(first_operand_token, line_number);
        instruction_line->second_operand_type = getOperandType(second_operand_token, line_number);
    }
    return true;
}

boolean firstPass(char* file_name, label_table_node* label_table_head, data_node** data_list_head, data_node** instruction_list_head, int* IC, int* DC){
    boolean is_label = false;
    char* line = NULL;
    char* am_file_name = NULL;
    FILE* am_file = NULL;
    char* label_name = NULL;
    boolean first_pass_found_error = false;
    int am_line_counter = 0;
    char* inline_string = NULL;
    char* inline_data = NULL;
    int i=0;
    data temp_word = {0};
    char* token = NULL;
    char* data_endptr = NULL;
    long temp_long = 0;
    instruction_line* temp_instruction_line = NULL;
    int L = 0;
    label_table_node* temp_label_table_entry_node = NULL;

    *IC = MEMORY_BASE_ADDRESS;
    *DC = 0;

    temp_word.type = WORD;
    
    line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    if(line == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }

    am_file_name = malloc((strlen(file_name) + sizeof(".am")) * sizeof(char));
    if(am_file_name == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    strcpy(am_file_name, file_name);
    am_file_name = strcat(am_file_name, ".am");

    am_file = fopen(am_file_name, "r");
    if(am_file == NULL){
        PRINT_ERR_OPEN_FILE_FAILED
        exit(1);
    }

    while(fgets(line, MAX_LINE_LENGTH, am_file) != NULL){
        am_line_counter++;
        is_label = isLabel(line, &label_name);
        if(isData(line, is_label) || isString(line, is_label)){
            if(is_label){
                if(isLegalLabelName(label_name)){
                    if (!addToLabelTable(label_table_head, label_name, *DC, DATA)){
                        FOUND_ERROR
                        PRINT_ERR_LABEL_ALREADY_DEFINED(label_name, am_line_counter)
                    }
                }
                else{
                    FOUND_ERROR
                    PRINT_ERR_ILLEGAL_LABEL_NAME(label_name, am_line_counter)
                }
            }
            if(isString(line, is_label)){
                inline_string = getString(line);
                if(inline_string == NULL){
                    FOUND_ERROR
                    PRINT_ERR_MISSING_QUOTES_IN_STRING_DEFINITION(am_line_counter)
                }
                else{
                    for(i=0; i < strlen(inline_string); i++){ /*loops over the word and adds each character to the list and increments *DC by 1*/
                        temp_word.content.word = inline_string[i];
                        addWordOrLabelToList(data_list_head, temp_word, *DC);
                        (*DC)++;
                    }
                    /*adds to the list the null terminatior and increments *DC by 1*/
                    temp_word.content.word = '\0';
                    addWordOrLabelToList(data_list_head, temp_word, *DC);
                    (*DC)++;
                    free(inline_string);
                }
            }
            else if(isData(line, is_label)){
                inline_data = getData(line);
                if(inline_data == NULL){
                    PRINT_ERR_MISSING_DATA_AFTER_INSTRUCTION(am_line_counter)
                    FOUND_ERROR
                }
                else{
                    token = strtok(inline_data, "," WHITESPACES);
                    if(token == NULL){
                        PRINT_ERR_ILLEGAL_DATA_AFTER_INSTRUCTION(am_line_counter)
                        FOUND_ERROR
                    }
                    while(token != NULL){
                        data_endptr = NULL;
                        temp_long = strtol(token, &data_endptr, DECIMAL);
                        if(errno == 0 && token != data_endptr && *data_endptr == '\0'){ /*checks if ther was an error in strtol*/
                            if(temp_long >= MINIMUM_12BITS_SIGNED && temp_long <= MAXIMUM_12BITS_SIGNED){
                                temp_word.content.word = temp_long;
                                addWordOrLabelToList(data_list_head, temp_word, *DC);
                                (*DC)++;
                                token = strtok(NULL, "," WHITESPACES);
                            }
                            else{
                                token = NULL;
                                FOUND_ERROR
                                PRINT_ERR_INVALID_NUMBER_IN_DATA(am_line_counter)
                            }
                        }
                        else{
                            token = NULL;
                            FOUND_ERROR
                            PRINT_ERR_ILLEGAL_DATA_AFTER_INSTRUCTION(am_line_counter)
                        }
                    }
                    free(inline_data);
                }
            }
        }
        else {
            if (isExtern(line, is_label)) {
                if(is_label){
                    FOUND_ERROR
                    PRINT_ERR_LABEL_BEFORE_EXTERN(am_line_counter)
                }
                else if(!addExternToTable(line, label_table_head)){
                    FOUND_ERROR
                    PRINT_ERR_MISSING_LABEL_AFTER_EXTERN(am_line_counter)
                }
            }
            else if(!isEntry(line, is_label)){/*ignoring entry lines in the first pass*/
                if(is_label){
                    if(!addToLabelTable(label_table_head, label_name, *IC, INSTRUCTION)){
                        FOUND_ERROR
                        PRINT_ERR_LABEL_ALREADY_DEFINED(label_name, am_line_counter);
                    }
                }
                temp_instruction_line = (instruction_line*)(malloc(sizeof(instruction_line)));
                if(temp_instruction_line == NULL){
                    PRINT_ERR_MEMORY_ALLOCATION_FAILED
                    exit(1);
                }
                if(parseInstruction(line, is_label, temp_instruction_line, am_line_counter)){
                    L = addInstructionLineToList(temp_instruction_line, instruction_list_head, *IC);
                    *IC += L;
                }
                else{
                    FOUND_ERROR
                }
                free(temp_instruction_line);
            }
        }
    }
    /*step 17 in the pseudo-code*/
    temp_label_table_entry_node = label_table_head;
    while(temp_label_table_entry_node != NULL){
        if(temp_label_table_entry_node->entry.type == DATA){
            temp_label_table_entry_node->entry.address += *IC;
        }
        temp_label_table_entry_node = temp_label_table_entry_node->next;
    }
    fclose(am_file);
    free(line);
    free(am_file_name);

    return !first_pass_found_error;
}