#include "general_assembler_functions.h"

#define DATA_INSTRUCTION_PREFIX ".data"
#define STRING_INSTRUCTION_PREFIX ".string"
#define ENTRY_INSTRUCTION_PREFIX ".entry"
#define EXTERN_INSTRUCTION_PREFIX ".extern"

boolean isSpecificInstruction(const char* line, const char* instruction_type, boolean is_label){
    char* line_duplicate = NULL;
    char* token = NULL;

    line_duplicate = strdup(line);
    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    
    if(is_label){
        token = strtok(line_duplicate, ":");
        token = strtok(NULL, WHITESPACES);
    }
    else{
        token = strtok(line_duplicate, WHITESPACES);
    }

    if(strcmp(token, instruction_type) == 0){
        free(line_duplicate);
        return true;
    }
    free(line_duplicate);
    return false;
}

boolean isData(const char* line, boolean is_label){
    return isSpecificInstruction(line, DATA_INSTRUCTION_PREFIX, is_label);
}

boolean isString(const char* line, boolean is_label){
    return isSpecificInstruction(line, STRING_INSTRUCTION_PREFIX, is_label);
}

boolean isEntry(const char* line, boolean is_label){
    return isSpecificInstruction(line, ENTRY_INSTRUCTION_PREFIX, is_label);
}

boolean isExtern(const char* line, boolean is_label){
    return isSpecificInstruction(line, EXTERN_INSTRUCTION_PREFIX, is_label);
}

/*checks if a line is a label and if label_name isn't NULL, allocates memory for label_name and copies the label name to there*/
boolean isLabel(const char* line, char** label_name){
    char* line_duplicate = NULL;
    char* token = NULL;
    int i=0;
    boolean found_label = false;

    line_duplicate = strdup(line);
    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }

    token = strtok(line_duplicate, WHITESPACES);
    if(token != NULL){
        for(i=1; i < strlen(token); i++){/*loops over the string and looks for the ':' char*/
            if (token[i] == ':') {
                token[i] = '\0';
                found_label = true;
            }
        }
        if(!found_label){
            return false;
        }
        if (label_name != NULL) {
            *label_name = malloc((strlen(token) + 1) * sizeof(char));
            if (*label_name == NULL) {
                PRINT_ERR_MEMORY_ALLOCATION_FAILED
                exit(1);
            }
            strcpy(*label_name, token);
        }
        free(line_duplicate);
        return true;
    }
    free(line_duplicate);
    return false;
}