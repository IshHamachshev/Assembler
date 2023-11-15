#include "label_table.h"

/*the function creates a label_table and return a pointer to it*/
label_table_node* createLabelTable(){
    label_table_node* head = (label_table_node*)malloc(sizeof(label_table_node)); /*allocating memory for the table*/

    if(head == NULL){ /*memory allocation failed*/
        PRINT_ERR_MEMORY_ALLOCATION_FAILED;
        exit(1);
    }
    else{ /*new_table intialization*/
        head->entry.address = 0;
        head->entry.label_name = NULL;
        head->entry.type = 0;
        head->next = NULL;
    }
    return head;
}

/*the function takes an existing table and frees it*/
void freeLabelTable(label_table_node* head){
    label_table_node* current_node = head;
    label_table_node* next_node = NULL;

    while(current_node != NULL){/*loops over the linked list and frees it*/
        next_node = current_node->next;
        free(current_node->entry.label_name);
        free(current_node);
        current_node = next_node;
    }
}


/*the function adds a label to the table*/
boolean addToLabelTable(label_table_node* head, char* label_name, unsigned short address, label_type type){
    label_table_node* node = NULL;

    if(head == NULL || label_name == NULL){/*checks if the input is valid*/
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    if(head->entry.label_name == NULL){ /*if the list is empty, initialize the first entry*/
        head->entry.label_name = label_name;
        head->entry.address = address;
        head->entry.type = type;
        head->next = NULL;
        head->entry.is_entry = false;
        return true; /*the function worked correctly*/
    }

    node = head; /*intialization of the node as the table head*/
    while(node->next != NULL){ /*loops untill the last node in the list*/
        if(strcmp(node->entry.label_name, label_name) == 0){ /*if the name of the label already exists*/
            return false;
        }
        node = node->next;
    }
    node->next = (label_table_node*)malloc(sizeof(label_table_node)); /*allocating memory for the new node*/
    if(node->next == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    node->next->entry.label_name = (char*)(malloc(sizeof(label_name))); /*allocating memory for the label name*/
    if(node->next->entry.label_name == NULL){
        PRINT_ERR_MEMORY_ALLOCATION_FAILED
        exit(1);
    }
    /*intialization of the new node*/
    node->next->next = NULL;
    node->next->entry.address = address;
    node->next->entry.label_name = label_name;
    node->next->entry.type = type;
    node->next->entry.is_entry = false;
    
    return true;
}


/*the function gets the entry that matches the label name, the function returns false if the label wasnt defined otherwise it will return true*/
boolean getEntry(label_table_node* head, const char* label_name, label_table_entry* matching_entry){
    label_table_node* node = head; /*intialization of the node as the table head*/

    if(head == NULL || label_name == NULL){/*makes sure the input is valid*/
        PRINT_ERR_INVALID_INPUT
        exit(1);
    }

    while(node != NULL){ /*loops over the list and checks each label_name if it matches the label_name from the input*/
        if(strcmp(node->entry.label_name, label_name) == 0){
            *matching_entry = node->entry;
            return true;
        }
        node = node->next;
    }

    return false; /*the label_name wasnt found in any of the entries*/
}