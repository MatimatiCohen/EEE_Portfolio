#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

//module for general functions to help parse and create files
#define NUM_COUNTERS_MAX 100

//structures

struct UserArgs{
    char *input_text_file;
    int threads_num;
    int num_counters;
    int log_enabled;
};
typedef struct UserArgs UserArgs;

//functions deceleration
int GetArgs(UserArgs *user_args, int argc,char** args);
void CreateCountersFile(char **argv, pthread_mutex_t files_mutex[NUM_COUNTERS_MAX]);
char* create_counter_file_name(char * file_name,int counter_num);
char *TruncateLastChar(char* str);
char *create_thread_file_name(char *file_name,int thread_num);
void create_thread_file(int i);
void create_file(char* file_name, bool init);

#endif