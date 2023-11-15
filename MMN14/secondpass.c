#include "secondpass.h"
#include "general_assembler_functions.h"

#define FOUND_ERROR second_pass_found_error = true;
#define PRINT_ERR_TEXT_AFTER_ENTRY(line_number) fprintf(stderr, ERROR "Extra text after entry declaration in line %d\n", line_number);
#define PRINT_ERR_MISSING_LABEL_AFTER_DOT_ENTRY(line_number) fprintf(stderr, ERROR "Missing label after .entry in line %d\n", line_number);
#define PRINT_ERR_LABEL_AFTER_DOT_ENTRY_DOESNT_EXIST(line_number) fprintf(stderr, ERROR "The label after instruction in line %d doesn't exist\n", line_number);
#define PRINT_ERR_LABEL_USED_DOESNT_EXIST(label_name) fprintf(stderr, ERROR "The label %s used doesn't exist\n", label_name);

/*the function takes an existing name for a file, and opens a new file with the added extension and the chosen mode*/
FILE* fopenWithExtension(char* file_name, char* file_name_extension, const char* mode);

/*the function sets the is_entry flag for a label in label_table to true if it matches the label_name, the function returns true if it worked otherwise returns false*/
boolean setIsEntryFlag(const char* label_name, label_table_node* label_table_head);

/* helper function - convert integer to base64 format*/
char intToBase64(int n);

/* convert endoed word to the base64 format */
char* wordToBase64(word word, char* encoded_word);

/*this function runs on the instruction list from the first pass and replaced the labels with its address and assign it the R flag of 'ARE' */
/* it replaces only the internal labels and not extern - extern will be handled later while bulding the external file - because the file must be created only if there are no errors before*/
/* and it should find all the extern refernces on the original list*/
/* if a label in the instruction list not found on the label table, it returns the error for prompting to the user and flag failure on the second pass in order not to create the output files*/
boolean updateLabelAddresses(label_table_node* label_table_head, data_node* instruction_list_head);


/* this function writes the ob file using the 12bit words in the instruction linked list and the data linked list*/
/* first it writes the Instruction and data length using the IC,DC and the MEMORY_BASE_ADDRESS value*/
/* then it converts and writes all the words in the instruction list to the file in the BASE64 encoding  */
/* after it converts and writes all the data words in the data linked list to the file in the BASE64 encoding  */
void writeObjectFile(char* file_name, data_node* data_list_head, data_node* instruction_list_head, int IC, int DC);

/* this function writes the external end entires list to the .ext and .ent files*/
/* enuamrting each label in the labels linked list */
/* a label that is flag as enrty is written to the entries file with its known address that was assigned in the begging of the pass*/
/* a label whitch is an exten label - the function find its instances on the instruction list and write its adresses to the extern file */
/* the files are created when the first label should be written (because if there is no external or entries the file shouldn't be creates)*/
void writeEntriesAndExternFiles(char* file_name, label_table_node* label_table_head, data_node* instruction_list_head);


/*the function takes an existing name for a file, and opens a new file with the added extension and the chosen mode*/
FILE* fopenWithExtension(char* file_name, char* file_name_extension, const char* mode){
    char* new_file_name = NULL;
    FILE* new_file = NULL;
    if(file_name == NULL || file_name_extension == NULL){
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    new_file_name = malloc((strlen(file_name) + sizeof(file_name_extension)) * sizeof(char));
    if(new_file_name == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }

    strcpy(new_file_name, file_name);
    new_file_name = strcat(new_file_name, file_name_extension);

    new_file = fopen(new_file_name, mode);
    if(new_file == NULL){
        PRINT_ERR_OPEN_FILE_FAILED
        exit(1);
    }
    free(new_file_name);
    return new_file;
}
/*the function sets the is_entry flag for a label in label_table to true if it matches the label_name, the function returns true if it worked otherwise returns false*/
boolean setIsEntryFlag(const char* label_name, label_table_node* label_table_head){
    label_table_node* node = NULL;

    node = label_table_head;
    while(node != NULL){/*loops over the label_table and looks for a matching label_name*/
        if(strcmp(label_name, node->entry.label_name) == 0 && (node->entry.type != EXTERN)){
            node->entry.is_entry = true;
            return true;
        }
        node = node->next;
    }
    return false;
}

char intToBase64(int n) {
    if (n >= 0 && n <= 25) {
        return 'A' + n;
    }
    else if (n >= 26 && n <= 51) {
        return 'a' + n - 26;
    }
    else if (n >= 52 && n <= 61) {
        return '0' + n - 52;
    }
    else if (n == 62) {
        return '+';
    }
    else if (n == 63) {
        return '/';
    }
    else {
        return '\0';
    }
}

char* wordToBase64(word word, char* encoded_word){
    encoded_word[0] = intToBase64((word.word >> 6) & 63);
    encoded_word[1] = intToBase64(word.word & 63);
    encoded_word[2] = '\0';
    return encoded_word;
}

boolean updateLabelAddresses(label_table_node* label_table_head, data_node* instruction_list_head) {

    data_node* node = NULL;
    label_table_entry label_table_entry = {0};
    boolean found_error = false;

    node = instruction_list_head;
    while(node != NULL){/*looping over the words in the list*/
        if (node->data.type == LABEL) {
            if (!getEntry(label_table_head, node->data.content.label, &label_table_entry)) {
                PRINT_ERR_LABEL_USED_DOESNT_EXIST(node->data.content.label)
                found_error = true;
            }
            else if (label_table_entry.type != EXTERN) {
                node->data.type = WORD;
                node->data.content.word = 2; /*R from ARE*/
                node->data.content.word |= (label_table_entry.address << 2);
            }
        }
        node = node->next;
    }

    return !found_error;
}

void writeObjectFile(char* file_name, data_node* data_list_head, data_node* instruction_list_head, int IC, int DC){
    FILE* ob_file = NULL;
    char encoded_word[3] = "\0";
    data_node* node = NULL;
    word word = {0};

    /*creating and opening the object file*/
    ob_file = fopenWithExtension(file_name, ".ob", "w");
    fprintf(ob_file, "%d %d\n", IC - MEMORY_BASE_ADDRESS, DC);

    node = instruction_list_head;
    while(node != NULL){/*looping over the words in the list*/
        if (node->data.type == LABEL) { /*Must be an external label*/
            word.word = 01; /*E from ARE*/
            fprintf(ob_file, "%s\n", wordToBase64(word, encoded_word));
        }
        else {
            word.word = node->data.content.word;
            fprintf(ob_file, "%s\n", wordToBase64(word, encoded_word));
        }
        node = node->next;
    }

    node = data_list_head;
    while(node != NULL){/*looping over the words in the list*/
        word.word = node->data.content.word;
        fprintf(ob_file, "%s\n", wordToBase64(word, encoded_word));
        node = node->next;
    }

    /*closing the object file*/
    fclose(ob_file);
}



void writeEntriesAndExternFiles(char* file_name, label_table_node* label_table_head, data_node* instruction_list_head){
    FILE* ent_file = NULL;
    FILE* ext_file = NULL;
    label_table_node* label_table_node_ptr = NULL;
    data_node* data_node_ptr = NULL;

    label_table_node_ptr = label_table_head;
    while (label_table_node_ptr != NULL) {
        if (label_table_node_ptr->entry.is_entry) {
            if (ent_file == NULL) {
                /*creating and opening the entry file*/
                ent_file = fopenWithExtension(file_name, ".ent", "w");
            }
            fprintf(ent_file, "%s\t%u\n", label_table_node_ptr->entry.label_name, label_table_node_ptr->entry.address);
        } else if (label_table_node_ptr->entry.type == EXTERN) {
            data_node_ptr = instruction_list_head;
            while (data_node_ptr != NULL) {
                if (data_node_ptr->data.type == LABEL && strcmp(data_node_ptr->data.content.label, label_table_node_ptr->entry.label_name) == 0) {
                    if (ext_file == NULL) {
                        /*creating and opening the external file*/
                        ext_file = fopenWithExtension(file_name, ".ext", "w");
                    }
                    fprintf(ext_file, "%s\t%u\n", label_table_node_ptr->entry.label_name, data_node_ptr->address);
                }
                data_node_ptr = data_node_ptr->next;
            }
        }
        label_table_node_ptr = label_table_node_ptr->next;
    }
    
    if (ent_file != NULL) {
        fclose(ent_file);
    }
    if (ext_file != NULL) {
        fclose(ext_file);
    }
}

/* this function implemnets the second pass */
/* it gets the original file and the linked list and tables created on the first pass and the IC and DC values*/
/* first it should finish collecting all the data needed to finish the process and then if there are no erroes it writing the files*/
/* thus, before building the files it :*/
/* 1. run on the orginal files, expanding its macros and look for entry lines*/
/* 2. update the instruction list - change the labels placeholer with its true address - and report error if a label not found in the labels list*/
/* after this steps all the data is in the linked list and tables and if there isn't any error before, it starts writing the data from the memory to files*/
/* 1. write the object file from the full instructions linked list and the data linked list*/
/* 2. bulid the entry file(if needed) and extern file (if needed) from the data in the labels list*/

int secondPass(char* file_name, label_table_node* label_table_head, data_node* data_list_head, data_node* instruction_list_head, int IC, int DC){
    int am_line_counter = 0;
    boolean is_label = false;
    boolean second_pass_found_error = false;
    char* line = NULL;
    char* label_name = NULL;
    FILE* am_file = NULL;

    line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    if(line == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    /*opening the expanded macros file*/
    am_file = fopenWithExtension(file_name, ".am", "r");

    while(fgets(line, MAX_LINE_LENGTH, am_file) != NULL){
        am_line_counter++;
        is_label = isLabel(line, NULL);
        if(isEntry(line, is_label)){ /*only entry lines are relevant for the second pass*/
            if(is_label){ /*gets the entry if there's a label*/
                label_name = strtok(line, ":" WHITESPACES);
                label_name = strtok(NULL, WHITESPACES);
                label_name = strtok(NULL, WHITESPACES);
            }
            else{/*gets the entry when there's no label*/
                label_name = strtok(line, WHITESPACES);
                label_name = strtok(NULL, WHITESPACES);
            }

            if(label_name == NULL){
                PRINT_ERR_MISSING_LABEL_AFTER_DOT_ENTRY(am_line_counter)
                FOUND_ERROR
            }
            if(strtok(NULL, WHITESPACES)){
                PRINT_ERR_TEXT_AFTER_ENTRY(am_line_counter)
                FOUND_ERROR
            }
            if(!setIsEntryFlag(label_name, label_table_head)){
                PRINT_ERR_LABEL_AFTER_DOT_ENTRY_DOESNT_EXIST(am_line_counter)
                FOUND_ERROR
            }
        }
    }
    free(line);
    if(second_pass_found_error){
        fclose(am_file);
        return false;
    }

    if(!updateLabelAddresses(label_table_head, instruction_list_head)) {
        return false;
    }

    writeObjectFile(file_name, data_list_head, instruction_list_head, IC, DC);
    writeEntriesAndExternFiles(file_name, label_table_head, instruction_list_head);

    fclose(am_file);
    return true;
}