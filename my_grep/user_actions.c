#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "user_actions.h"


void initialize_actions(user_actions *actions){
    actions->file_name=NULL;
    initialize_flag(&actions->A);
    actions->rows_after=0;
    actions->rows_left_to_print=-1;
    initialize_flag(&actions->b);
    initialize_flag(&actions->c);
    initialize_flag(&actions->i);
    initialize_flag(&actions->n);
    initialize_flag(&actions->v);
    actions->non_valid_rows_num=0;
    initialize_flag(&actions->x);
    actions->exact_match=false;
    initialize_flag(&actions->E);
    actions->valid_rows_num=0;
    actions->sub_string=NULL;
    actions->line_count=0;
    actions->bytes_count=0;
    actions->regex_len=0;
}


void read_user_actions(int argc, char *argv[],user_actions *actions) {
    int i;
    bool sub_string_found = false;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-A")) {
            activate(&actions->A);
            i++;
            assert(is_num(argv[i]));
            actions->rows_after = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-b")) {
            activate(&actions->b);
        } else if (!strcmp(argv[i], "-c")) {
            activate(&actions->c);
        } else if (!strcmp(argv[i], "-i")) {
            activate(&actions->i);
        } else if (!strcmp(argv[i], "-n")) {
            activate(&actions->n);
        } else if (!strcmp(argv[i], "-v")) {
            activate(&actions->v);
        } else if (!strcmp(argv[i], "-x")) {
            activate(&actions->x);
        } else if (!strcmp(argv[i], "-E")) {
            activate(&actions->E);
            i++;
            actions->sub_string = argv[i];
          //  assert(find_escape_chars(actions->sub_string)&&is_char_escaped(actions->sub_string)); //TODO: fix this handle, do it somewhere else
            sub_string_found = true;
        } else if (!sub_string_found) {
            if (!actions->E.is_on){
                assert(!find_escape_chars(argv[i]));
            }
            actions->sub_string = argv[i];
            sub_string_found = true;
        } else if (actions->file_name==NULL) {
            assert((argc - 1) == i);
            actions->file_name = argv[i];
        }
    }

}
