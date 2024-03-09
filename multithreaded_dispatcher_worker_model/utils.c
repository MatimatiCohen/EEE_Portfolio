#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_COUNTERS_MAX 100



int GetArgs(UserArgs *user_args,int argc, char** args)
{
    if (argc!=5) return 0;
    user_args->input_text_file=args[1];
    user_args->threads_num=atoi(args[2]);
    user_args->num_counters=atoi(args[3]);
    user_args->log_enabled=atoi(args[4]);
    return 1;
}

void CreateCountersFile(char **argv, pthread_mutex_t files_mutex[NUM_COUNTERS_MAX]){
    char file_name[13]; 
    int i=0;
    for(i; i<atoi(argv[3]);i++){
        create_counter_file_name(file_name, i);
        create_file(file_name, true);
        // files_mutex[i] = PTHREAD_MUTEX_INITIALIZER; //init file mutex
        pthread_mutex_init(&files_mutex[i], NULL);
    }
}

void create_file(char* file_name, bool init)
{
    FILE *fp;
    fp=fopen(file_name,"w");
    assert(fp!=NULL);
    if (init)
    {
        long long int num=0;
        fprintf(fp,"%lld\n",num);
    }
    fclose(fp);
}

char *create_counter_file_name(char *file_name,int counter_num)
{  
    sprintf(file_name,"count%02d.txt",counter_num);
    return file_name;
}

char *create_thread_file_name(char *file_name,int thread_num)
{  
    sprintf(file_name,"thread%02d.txt",thread_num);
    return file_name;
}

void create_thread_file(int id)
{
    char file_name[13];
    create_thread_file_name(file_name, id);
    create_file(file_name, false);
    
}

char* TruncateLastChar(char* str)
{
    int n=0;
    n = strlen(str);
    str[n-1] = '\0';
    return str;
}
