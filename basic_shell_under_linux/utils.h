#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdio.h>

//structures
struct Line_Data
{
    char * str;
    size_t line_size;
};
typedef struct Line_Data line_data;

//functions deceleration
void init_line(line_data * line);
char* CopyString(char *original_string);
void TruncateLastChar();
void free_line(line_data  line);
void undo_strtok(line_data *line);

#endif