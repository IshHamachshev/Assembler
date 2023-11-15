#include "globals.h"
#include "macro.h"
#include "label_table.h"
#include "firstpass.h"
#include "secondpass.h"

int main(int argc , char **argv){
    int i = 0;
    int IC = 0;
    int DC = 0;
    boolean not_found_error_main = true;
    label_table_node* label_table_head = NULL;
    data_node* data_list_head = NULL;
    data_node* instruction_list_head = NULL;
    label_table_node* curr_label_table_node = NULL;
    label_table_node* prev_label_table_node = NULL;
    data_node* curr_instruction_list_node = NULL;
    data_node* prev_instruction_list_node = NULL;
    data_node* curr_data_list_node = NULL;
    data_node* prev_data_list_node = NULL;

    if(argc <= 1){
        puts("Usage: ./Assembler [files]");
        return 1;
    }
    for(i = 1; i < argc; i++){
    /*initializing all the variables*/
    IC = 0;
    DC = 0;
    not_found_error_main = true;
    label_table_head = NULL;
    data_list_head = NULL;
    instruction_list_head = NULL;
    curr_label_table_node = NULL;
    prev_label_table_node = NULL;
    curr_instruction_list_node = NULL;
    prev_instruction_list_node = NULL;
    curr_data_list_node = NULL;
    prev_data_list_node = NULL;

        label_table_head = malloc(sizeof(label_table_node));
        if (label_table_head == NULL) {
            PRINT_ERR_MEMORY_ALLOCATION_FAILED
            exit(1);
        }
        label_table_head->entry.label_name = NULL;
        not_found_error_main = true;

        fprintf(stdout, "--- RUNNING FILE %s ---\n", argv[i]);
        not_found_error_main = expandMacros(argv[i]);
        if(not_found_error_main){
            fprintf(stdout, "--- RUNNING FIRST PASS ---\n");
            not_found_error_main = firstPass(argv[i], label_table_head, &data_list_head, &instruction_list_head, &IC, &DC);

        }
        if(not_found_error_main){
            fprintf(stdout, "--- RUNNING SECOND PASS ---\n");
            secondPass(argv[i], label_table_head, data_list_head, instruction_list_head, IC, DC);
        }
        fprintf(stdout, "--- FINISHED RUNNING FILE %s ---\n\n", argv[i]);



        curr_label_table_node = label_table_head;
        while(curr_label_table_node != NULL) {
            prev_label_table_node = curr_label_table_node;
            curr_label_table_node = curr_label_table_node->next;
            free(prev_label_table_node->entry.label_name);
            free(prev_label_table_node);
        }

        curr_instruction_list_node = instruction_list_head;
        while(curr_instruction_list_node != NULL) {
            prev_instruction_list_node = curr_instruction_list_node;
            curr_instruction_list_node = curr_instruction_list_node->next;
            if (prev_instruction_list_node->data.type == LABEL) {
                free(prev_instruction_list_node->data.content.label);
            }
            free(prev_instruction_list_node);
        }
        
        curr_data_list_node = data_list_head;
        while(curr_data_list_node != NULL) {
            prev_data_list_node = curr_data_list_node;
            curr_data_list_node = curr_data_list_node->next;
            if (prev_data_list_node->data.type == LABEL) {
                free(prev_data_list_node->data.content.label);
            }
            free(prev_data_list_node);
        }

    }
    return 0;
}