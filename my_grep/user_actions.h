#ifndef USER_ACTIONS_H
#define USER_ACTIONS_H
#include "string_checks.h"
#include "flag_of_action.h"


typedef struct User_Actions{
    flag_of_action A;
    int rows_after;
    int rows_left_to_print;
    flag_of_action b;
    int bytes_count;
    flag_of_action c;
    int valid_rows_num;
    flag_of_action i;
    flag_of_action n;
    int line_count;
    flag_of_action v;
    int non_valid_rows_num;
    flag_of_action x;
    bool exact_match;
    flag_of_action E;
    char *sub_string;
    char *file_name;
    int regex_len;
} user_actions;


void initialize_actions(user_actions *actions);
void read_user_actions(int argc, char *argv[],user_actions *actions);






#endif
