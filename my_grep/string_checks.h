#ifndef STRING_CHECKS_H
#define STRING_CHECKS_H


#include <stdbool.h>

bool is_num(char* string);
void lower_string(char* string);
void copy_line(char *line, char** copy);
bool new_line_char_exits(char* line);
bool find_escape_chars(char* string);
bool is_char_escaped(char *string);


#endif
