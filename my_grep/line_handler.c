#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "line_handler.h"
#include <stdbool.h>
#include <assert.h>

bool is_regex_in_line(Reg_Exp *regex_string, char *line);
bool is_match_at_place(Reg_Exp *regex_string, char *line);



int read_line(FILE *f,char** line){
    size_t n = 0;
    return getline(line, &n, f);
}



void is_match_in_line(char* line, user_actions *actions){
    if (actions->i.is_on) {
        lower_string(actions->sub_string);
    }
    handle_cases(line, actions);
}



void handle_cases(char *line, user_actions * actions){
    bool found_sub_str=false;
    char *lower_case_line=NULL;
    copy_line(line,&lower_case_line);
    lower_string(lower_case_line);
    if (actions->i.is_on){
        found_sub_str = report_line_match(lower_case_line, actions);
    }
    else {
        found_sub_str = report_line_match(line, actions);
    }
    if ((found_sub_str)&&(!actions->v.is_on)&&(!actions->c.is_on)) {
        print_line(line,actions);
    }
    else if ((!found_sub_str)&&(actions->v.is_on)&&(!actions->c.is_on)){
        print_line(line,actions);
    }
    if ((!found_sub_str)&&(!actions->v.is_on)&&(!actions->c.is_on)&&(actions->rows_left_to_print>0)){
        actions->rows_left_to_print--;
        print_line(line,actions);
    }
    if (lower_case_line){
        free(lower_case_line);
    }
}


bool report_line_match(char *line,user_actions *actions){
    bool found_sub_str=false;
    int line_length=strlen(line)-1;
    found_sub_str= str_match_regex(line,actions);
    if (actions->x.is_on){
      if (!actions->E.is_on){
        if (!strncmp(line,actions->sub_string, strlen(line)-1)) {
            actions->exact_match = true;
        } //TODO: add a case in which E is on, x is on  and regular value is given
      }
      else {
        if (found_sub_str&& (line_length==actions->regex_len)){
        actions->exact_match=true;
        }
      }
    }
    if (found_sub_str){
        actions->valid_rows_num++;
    }
    else {
        actions->non_valid_rows_num++;
    }
    if (actions->A.is_on&&(found_sub_str||actions->exact_match)){
      if (actions->rows_left_to_print==0){
	printf("--\n");
      }
        actions->rows_left_to_print=actions->rows_after;
    }
    return found_sub_str;
}


bool str_match_regex(char* line, user_actions *actions) {
    char *found_sub_str = NULL;
    bool found_sub_regex;
    Reg_Exp *regex_string;
    if (!actions->E.is_on) { //no regular expression
        found_sub_str = strstr(line, actions->sub_string);
        if (found_sub_str) {
            return true;
        } else {
            return false;
        }
    }
    else{
        regex_string = (Reg_Exp *) malloc((1 + strlen(actions->sub_string) * sizeof(Reg_Exp)));
        assert(regex_string!=NULL);
        set_reg_expression(regex_string,actions->sub_string);
        found_sub_regex = is_regex_in_line(regex_string, line);
        actions->regex_len=reg_exp_len(regex_string);
        free(regex_string);
        return found_sub_regex;
    }

}



bool is_regex_in_line(Reg_Exp *regex_string, char *line){
    int i;
    int line_length= strlen(line);
    for (i=0; i<line_length;i++){
        if (is_match_at_place(regex_string,line +i)){
            return true;
        }
    }
    return false;
}


bool is_match_at_place(Reg_Exp *regex_string, char *line){
    if (end_of_reg_exp(regex_string)){
        return true;
    }
    if (*line=='\0'){
        return false;
    }
    bool left_match_option,right_match_option;
    switch (regex_string->type) {
        case CHAR:
            if (regex_string->data.single_char!=*line){
                return false;
            }
            return is_match_at_place(regex_string+1,line+1);
        case DOT:
            return is_match_at_place(regex_string+1,line+1);
        case BRACKETS:
            if (!(regex_string->data.bra.low_str<=*line && regex_string->data.bra.high_str>=*line)){
                return false;
            }
            return is_match_at_place(regex_string+1,line+1);
        case PARENTHESIS:
            left_match_option=(!strncmp(regex_string->data.par.left_str,line,regex_string->data.par.left_len)&&
                               is_match_at_place(regex_string+1,line+regex_string->data.par.left_len));
            right_match_option=(!strncmp(regex_string->data.par.right_str,line,regex_string->data.par.right_len)&&
                                is_match_at_place(regex_string+1,line+regex_string->data.par.right_len));
            if (left_match_option){
              regex_string->data.par.x_on_left_len=true;
              regex_string->data.par.x_on_right_len=false;
            }
            if (right_match_option){
              regex_string->data.par.x_on_right_len=true;
              regex_string->data.par.x_on_left_len=false;
            }
            return left_match_option||right_match_option;
        default:
            assert(false);
    }
}


void print_line(char* line, user_actions * actions){
    if (actions->n.is_on) {
      if (actions->A.is_on){
        if (actions->rows_left_to_print==actions->rows_after) {
            printf("%d:", actions->line_count);
        }
        else {
            printf("%d-", actions->line_count);
        }
      }else{
	printf("%d:", actions->line_count);
      }
    }
    if (actions->b.is_on) {
      if (actions->A.is_on){
        if (actions->rows_left_to_print==actions->rows_after) {
            printf("%d:", actions->bytes_count);
        } else {
            printf("%d-", actions->bytes_count);
        }
      }else{
	printf("%d:", actions->bytes_count);
      }
    }
    if (!actions->x.is_on) {
        printf("%s", line);
    }
    if ((actions->x.is_on) &&(actions->exact_match)){
        printf("%s", line);
        actions->exact_match=false;
    }

}
