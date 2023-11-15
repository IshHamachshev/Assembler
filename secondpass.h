#ifndef SECONDPASS_H
#define SECONDPASS_H

#include "globals.h"
#include "label_table.h"

int secondPass(char* file_name, label_table_node* label_table_head, data_node* data_list_head, data_node* instruction_list_head, int IC, int DC);

#endif /*SECONDPASS_H*/