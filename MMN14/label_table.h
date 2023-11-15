#ifndef LABEL_TABLE_H
#define LABEL_TABLE_H

#include "globals.h"

typedef enum {
    DATA,
    INSTRUCTION,
    EXTERN
} label_type;

typedef struct {
    char* label_name;
    unsigned short address; /*enough bits to access memory of 1024 words*/
    label_type type;
    boolean is_entry;
} label_table_entry;

typedef struct label_table_node {
    label_table_entry entry;
    struct label_table_node* next;
} label_table_node;


/*the function creates the table head and returns a pointer to it*/
label_table_node* createLabelTable();

/*the function takes an existing table and frees it*/
void freeLabelTable(label_table_node* head);

/*the function adds a label to the table*/
boolean addToLabelTable(label_table_node* head, char* label_name, unsigned short address, label_type type);

/*the function gets the entry that matches the label name, the function returns false if the label wasnt defined otherwise it will return true*/
boolean getEntry(label_table_node* head, const char* label_name, label_table_entry* matching_entry);

#endif /*LABEL_TABLE_H*/