#ifndef JOBS_H
#define JOBS_H

#define MAX_BG_PROCS 4
#define MAX_ARGS 64 

#include <stdbool.h>
#include <unistd.h>
#include "utils.h"

//structures
struct Background_Job
{
    int pid;
    line_data command_line_copy;
    char *args[MAX_ARGS];
} ;
typedef struct Background_Job bg_job;

struct Jobs_Manager
{
    bg_job *bg_job_arr;
    int jobs_counter;
    bool status[MAX_BG_PROCS];
} ;
typedef struct Jobs_Manager jobs_manager;

//functions deceleration
bool IsBackground(char **args);
void PrintJobs();
int UpdateJobs(jobs_manager *jm,pid_t pid,char **args,line_data *original_line);
void InitJobsManager(jobs_manager *jm);
void InitJob(bg_job *job);
void BackgroundJobsPoll(jobs_manager *jm);
void ClearJob(jobs_manager *jm,int index);

#endif