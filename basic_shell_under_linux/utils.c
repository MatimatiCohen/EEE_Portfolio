#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

//functions implementation
char* CopyString(char *original_string)
{   
    char *ret=NULL;
    char* copy=NULL;
    int len=strlen(original_string);
    copy=(char*)malloc(sizeof*copy*(len+1));
    assert(copy!=NULL);
    ret=copy;
    while(*original_string){
        *copy++=*original_string++;
    }
    return ret;
}

void TruncateLastChar(char* str)
{
    int n=0;
    n = strlen(str);
    str[n-1] = '\0';
}

void init_line(line_data * line)
{
    line->str=NULL;
    line->line_size=0;
}

void free_line(line_data line)
{

    if(line.str!=NULL)
    {
        free(line.str);
    }
}

void undo_strtok(line_data *line){

       int i;
    for (i=0;i<line->line_size;i++)
    {
        if (line->str[i]=='\0') line->str[i]=' ';
    }
}