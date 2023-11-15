#include "macro.h"

int line_counter;
boolean macros_expansion_found_error;

/*Struct of a linked list of lines that defines a macro*/
typedef struct{
    char name[MAX_LINE_LENGTH];
    char *content;
} Macro;

/*Errors definition for macros*/
#define PRINT_ERR_LINE_TOO_LONG(line_number) fprintf(stderr, ERROR "Line %d is too long\n", line_number);
#define PRINT_ERR_TEXT_AFTER_MCRO_DEFINITION(line_number) fprintf(stderr, ERROR "text after the beginning of a macro definition in line %d\n", line_number);
#define PRINT_ERR_TEXT_AFTER_ENDMCRO(line_number) fprintf(stderr, ERROR "There's text after the endmcro in line %d\n", line_number);
#define PRINT_ERR_ILLEGAL_MACRO_NAME(macro_name, line_number) fprintf(stderr, ERROR "the macro %s in line %d is not a legal macro name\n", macro_name, line_number);
#define PRINT_ERR_MACRO_ALREADY_DEFINED(macro_name, line_number) fprintf(stderr, ERROR "The macro %s in line %d is already defined\n", macro_name, line_number);

/*checks if a line is only whitespaces*/
boolean isLineWhite(char* line);

/*checks if a line is a comment*/
boolean isLineComment(char* line);

/*checks if it is a start of a macro, if it's a start of a macro it will save the name of the macro in macro_name*/
boolean isStartMacro(char* line, char** macro_name);

/*checks if it is the end of a macro*/
boolean isEndMacro(char* line);

/*checks if the name of the macro is valid*/
boolean isValidMacroName(const char* name);

/*checks if the length of the line isnt more then 81 (including the new line character)*/
boolean isValidLineLength(char* line);

/*adds a macro to the macro table*/
void addMacroToTable(mtable* macro_table, char* macro_name, FILE* macro_begin);

boolean isValidMacroName(const char* macro_name){
    int i;
    const char* illegal_macro_names_list[] = {"mcro", "endmcro", ".data", ".string", ".entry", ".extern",
                                              "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
                                              "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    const int list_length = 22;
    for(i=0; i < list_length ; i++){
        if(strcmp(macro_name, illegal_macro_names_list[i]) == 0){
            PRINT_ERR_ILLEGAL_MACRO_NAME(macro_name, line_counter)
            return false;
        }
    }
    return true; 
}

boolean isValidLineLength(char* line){
    if(strlen(line) >= MAX_LINE_LENGTH-1 && line[strlen(line) - 1] != '\n'){ /*checks if the line is over 81 symbols without new-line char*/
        return false;
    }
    return true;
}

boolean isLineComment(char* line){
    if(line == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
    if(line[0] == ';'){/*checks if the first character is ;*/
        return true;
    }
    return false;
}

boolean isLineWhite(char* line){
    int i;
    if(line == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
    for(i=0; i<strlen(line); i++){/*loops over the line and checks if there's a non whitespace character*/
        if(line[i] != ' ' && line[i] != '\t' && line[i] != '\n'){
            return false;
        }
    }
    return true;
}

boolean isStartMacro(char* line, char** macro_name){
    char* line_duplicate;
    char* token;
    if(line == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    line_duplicate = strdup(line);
    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    token = strtok(line_duplicate, INLINE_WHITESPACES);
    if(token == NULL){
        return false;
    }
    if(strcmp(token,"mcro") == 0){
        token = strtok(NULL, WHITESPACES);
        if(isValidMacroName(token)){
            *macro_name = strdup(token);
            if(*macro_name == NULL){
                PRINT_ERR_MEMORY_ALLOCATION_FAILED
                exit(1);
            }
            token = strtok(NULL, WHITESPACES);
            if(token != NULL){/*if there is a text after the macro name*/
                PRINT_ERR_TEXT_AFTER_MCRO_DEFINITION(line_counter)
                macros_expansion_found_error=true;
                free(line_duplicate);
                return true;
            }
        }
        else{
            free(line_duplicate);
            macros_expansion_found_error = true;
            return false;
        }
        free(line_duplicate);
        return true;
    }
    free(line_duplicate);
    return false;
}

boolean isEndMacro(char* line){
    char* line_duplicate;
    char* token;
    line_duplicate = strdup(line);
    if(line_duplicate == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    token = strtok(line_duplicate, WHITESPACES);
    if(token == NULL){
        free(line_duplicate);
        return false;
    }
    if(strcmp(token,"endmcro") == 0){
        token = strtok(NULL, WHITESPACES);
        if(token  == NULL){
            free(line_duplicate);
            return true;
        }
        else{
            PRINT_ERR_TEXT_AFTER_ENDMCRO(line_counter)
            macros_expansion_found_error=true;
            free(line_duplicate);
            return true;
        }
    }
    free(line_duplicate);
    return false;
}

void addMacroToTable(mtable* macro_table, char* macro_name, FILE* file_ptr){
    char* line = malloc(MAX_LINE_LENGTH * sizeof(char));
    char c;
    boolean initialization_worked = false;
    if(line == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    initialization_worked = initMacro(macro_table, macro_name);
    if(!initialization_worked){
        PRINT_ERR_MACRO_ALREADY_DEFINED(macro_name, line_counter)
        macros_expansion_found_error = true;
        return;
    }
    while(fgets(line, MAX_LINE_LENGTH, file_ptr) != NULL){
        line_counter++;
        if(isValidLineLength(line)){
            if(!isLineWhite(line) && !isLineComment(line)){
                if(isEndMacro(line)){
                    break;
                }
                addToTable(macro_table, macro_name, line);
            }
        }
        else{
            PRINT_ERR_LINE_TOO_LONG(line_counter)
            macros_expansion_found_error = true;
            /*skip all the characters till the line ends*/
            c=fgetc(file_ptr);
            while(c != EOF && c != '\n'){
                c = fgetc(file_ptr);
            }
        }
    }
    free(line);
}

boolean expandMacros(const char* file_name){
    /*declaring the variables , mallocing memory and intializing variables*/
    char* line = malloc(MAX_LINE_LENGTH * sizeof(char));
    FILE* as_file = NULL;
    FILE* am_file = NULL;
    char* as_file_name = malloc((strlen(file_name) + sizeof(".as")) * sizeof(char));
    char* am_file_name = malloc((strlen(file_name) + sizeof(".am")) * sizeof(char));
    char* macro_name = NULL;
    char* token = NULL;
    char* line_duplicate = NULL;
    line_node* macro_node = NULL;
    mtable* macro_table = NULL;
    char c;
    line_counter=0;
    macros_expansion_found_error=false;
    
    
    if(as_file_name == NULL || am_file_name == NULL){ /*memory allocation failed for one of the file names*/
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }

    macro_table = createTable(); 

    /*building the file names*/
    strcpy(as_file_name, file_name);
    strcpy(am_file_name, file_name);
    as_file_name=strcat(as_file_name, ".as");
    am_file_name=strcat(am_file_name, ".am");
    /*opening the files*/
    as_file=fopen(as_file_name, "r");
    am_file=fopen(am_file_name, "w");
    if(am_file == NULL || as_file == NULL){
        PRINT_ERR_OPEN_FILE_FAILED
        exit(1);
    }

    while(fgets(line, MAX_LINE_LENGTH, as_file) != NULL){/*while there are still lines to read*/
        line_counter++; 
        if(isValidLineLength(line)){/*checks that the line isnt too long*/
            macro_name=NULL;
            if(!isLineComment(line) && !isLineWhite(line)){/*makes sure the line is relevant*/
                if(isStartMacro(line, &macro_name)){/*checks if it is a start of a macro and if it is it adds it to the table*/
                    addMacroToTable(macro_table, macro_name, as_file);
                }
                else{/*check if the first token is an existing macro , if it is it will handle it*/
                    line_duplicate = strdup(line);
                    if (line_duplicate == NULL){
                        PRINT_ERR_MEMORY_ALLOCATION_FAILED
                        exit(1);
                    }
                    token = strtok(line_duplicate, WHITESPACES);
                    if(getValue(macro_table, token) != NULL){ /*putting in the file all the macro's content*/
                        macro_node = getValue(macro_table, token);
                        while(macro_node != NULL){
                            fputs(macro_node->line, am_file);
                            macro_node = macro_node->next;
                        }
                    }
                    else{/*handles the case when the line is neither a start of a macro or a macro*/
                        fputs(line, am_file);
                    }
                    free(line_duplicate);
                }
            }
        }
        else{/*if the line is too long*/
            PRINT_ERR_LINE_TOO_LONG(line_counter)
            macros_expansion_found_error = true;
            /*skip all the characters till the line ends*/
            c=fgetc(as_file);
            while(c != EOF && c != '\n'){
                c = fgetc(as_file);
            }
        }
    }
    
    free(line);
    fclose(as_file);
    fclose(am_file);
    free(as_file_name);
    if(macros_expansion_found_error){
        remove(am_file_name);
        free(am_file_name);
        return false;
    }
    return true;
}