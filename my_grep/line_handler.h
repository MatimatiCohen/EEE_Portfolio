#ifndef LINE_HANDLER_H
#define LINE_HANDLER_H


#include "string_checks.h"
#include "reg_exp.h"
#include "user_actions.h"


int read_line(FILE *f,char **line);
void is_match_in_line(char *line,user_actions *actions);
bool report_line_match(char *line,user_actions *actions);
void handle_cases(char *line, user_actions * actions);
void print_line(char* line, user_actions * actions);
bool str_match_regex(char* line, user_actions *actions);


#endif
