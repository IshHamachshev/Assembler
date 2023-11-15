#include "mtable.h"
mtable* createTable() {
    mtable* newTable = (mtable*)malloc(sizeof(mtable));
    if (newTable != NULL) { /*Memory allocation worked*/
        newTable->entries = NULL;
        newTable->size = 0;
    }
    else{ /*Memory allocation failed*/
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    return newTable;
}

/* Helper function to find the index of a key in the mtable, if the key is not in the table it will return -1*/
int findKeyIndex(mtable* mtable, const char* key) {
    int i; 
    for (i=0; i < mtable->size; i++) {/*loops over the table entries and checks where and if the macro is in the table*/
        if (strcmp(mtable->entries[i].macro_name, key) == 0) {
            return i;
        }
    }
    return -1; /*the key wasn't found*/
}

boolean initMacro(mtable* mtable, const char* macro_name){
    mtable_entry* newData;
    if(findKeyIndex(mtable,macro_name) >= 0){
        return false;
    }
    if (mtable == NULL || macro_name == NULL){/*Invalid input*/
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
    newData = (mtable_entry*)realloc(mtable->entries, (mtable->size + 1) * sizeof(mtable_entry));
    if (newData == NULL) {
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    mtable->entries = newData;
    mtable->entries[mtable->size].macro_name = strdup(macro_name);
    if(mtable->entries[mtable->size].macro_name == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    mtable->entries[mtable->size].lines_head = NULL;
    mtable->size++;
    return true;
} 


boolean addToTable(mtable* mtable, const char* key, const char* value) {
    line_node* newNode;
    line_node* current;
    int index;

    if (mtable == NULL || key == NULL || value == NULL){/*Invalid input*/
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
    index = findKeyIndex(mtable, key);

    newNode = (line_node*)malloc(sizeof(line_node));
    if (newNode == NULL) {
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    newNode->line = strdup(value);
    if(newNode->line == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    newNode->next = NULL;

    if(mtable->entries[index].lines_head == NULL){/*if the macro was only initialized*/
        mtable->entries[index].lines_head = newNode;
    }
    else{
        current = mtable->entries[index].lines_head;
        while(current->next != NULL){ /*loops through the linked list untill the last line*/
            current=current->next;
        }
        current->next = newNode;
    }
    return true; /*Succes*/
}

line_node* getValue(mtable* mtable,const char* key) {
    int index;
    if (mtable == NULL || key == NULL) {
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    index = findKeyIndex(mtable, key);
    if(index == -1){/*key not found*/
        return NULL;
    }
    if(mtable->entries[index].lines_head == NULL){/*handles the case that the macro is empty*/
        mtable->entries[index].lines_head = (line_node*)malloc(sizeof(line_node));
        if(mtable->entries[index].lines_head == NULL){
            PRINT_ERR_MEMORY_ALLOCATION_FAILED
            exit(1);
        }
        mtable->entries[index].lines_head->line = (char*)malloc(sizeof(char));
        mtable->entries[index].lines_head->line[0] = '\0';
        if(mtable->entries[index].lines_head->line == NULL){
            PRINT_ERR_MEMORY_ALLOCATION_FAILED
            exit(1);
        }
        mtable->entries[index].lines_head->next = NULL;
    }

    if (index >= 0){ /*returns a valid macro*/
        return mtable->entries[index].lines_head;
    }
    /* Key not found*/
    return NULL;
}

void freeTable(mtable* mtable) {
    if (mtable != NULL) {
        int i;
        line_node* current_node;
        line_node* next_node;

        for (i = 0; i < mtable->size; i++) {/*loops over the mtable entries anf frees it*/
            free(mtable->entries[i].macro_name);
            current_node = mtable->entries[i].lines_head;
            while(current_node != NULL){/*loops over the linked list and frees it*/
                next_node = current_node->next;
                free(current_node->line);
                free(current_node);
                current_node=next_node;
            }
        }
        free(mtable->entries);
        free(mtable);
    }
    else{
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }
}