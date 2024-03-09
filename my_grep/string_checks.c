#include "string_checks.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


bool is_num(char* string){
    while (*string != '\0'){
        if (!isdigit(*string)){
            return false;
        }
        string++;
    }
    return true;
}


void lower_string(char* string){
    while (*string != '\0'){
        *string= tolower(*string);
        string++;
    }
}

void copy_line(char *line, char** copy){
    *copy=(char*)malloc((strlen(line)+1)*sizeof(char));
    assert(*copy!=NULL);
    strcpy(*copy,line);
}

bool new_line_char_exits(char* line){
    while (*line!='\0'){
        if ((*line)=='\n'){
            return true;
        }
        line++;
    }
    return false;
}

bool find_escape_chars(char* string) {
    char special_chars[] = "|\\{}[].";
    char *find = strpbrk(string, special_chars);
    if (find) {
        return true;
    }
    return false;
}


bool is_char_escaped(char *string){
    char special_chars[] = "|\\{}[].";
    char *find = strpbrk(string, special_chars);
    int backslash_index=find-string;
    if (string[backslash_index]=='\\'){
        memmove(&string[backslash_index],&string[backslash_index+1],strlen(string)-backslash_index);
        return true;
    }
    return false;
}


