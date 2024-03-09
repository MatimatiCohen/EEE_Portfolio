#include "jobs.h"
#include "shell.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

//constants
#define MAX_BG_PROCS 4
#define MAX_ARGS 64 //according to Gadi's response in the forum

//functions implementation
bool IsBackground(char **args)
{
    int args_counter=arg_count(args);
    size_t last_arg_len=strlen(args[args_counter-1]);
    if (!strcmp(args[args_counter-1],"&")) //if last argument is & it's background process
    { 
        args[args_counter-1]=NULL;
        return true;
    }

    if (args[args_counter-1][last_arg_len-1]=='&')// if last char of last argument is & it's also a background process
    {
        args[args_counter-1][last_arg_len-1]='\0';
        return true;
    }
    return false;
}

void PrintJobs(bg_job* bg_jobs_arr,int jobs_number,jobs_manager *jm)
{
    for (int i=0; i < jobs_number; i++)
    {
        if ((bg_jobs_arr[i].pid)!=0)
        { 
            printf("%d\t",bg_jobs_arr[i].pid);
            int args_counter=arg_count(bg_jobs_arr[i].args);
            for (int j=0 ; j<(args_counter); j++)
            {
                printf("%s ",bg_jobs_arr[i].args[j]);
            }
            printf("\n");        
        }       
    }
}

void update_jm_counter(jobs_manager *jm)
{
    int i=0;
    int count=0;
    for(i;i<MAX_BG_PROCS;i++)
    {
        if (jm->status[i]==1)
        {
            count++;
        }
    }
    jm->jobs_counter=count;
}

int UpdateJobs(jobs_manager *jm,pid_t pid,char **args,line_data *original_line)
{ 
    update_jm_counter(jm);
    if (jm->jobs_counter<MAX_BG_PROCS)
    {
        for (int i = 0; i< MAX_BG_PROCS ; i++) 
        {
            if (jm->status[i] == 0) { //spot is free
                jm->status[i]=1;
                jm->bg_job_arr[i].pid=pid;
                jm->bg_job_arr[i].command_line_copy.str =CopyString(original_line->str);
                jm->bg_job_arr[i].command_line_copy.line_size=original_line->line_size;
                GetArgs(jm->bg_job_arr[i].command_line_copy.str,jm->bg_job_arr[i].args);
             
                int args_counter=arg_count(jm->bg_job_arr[i].args);
                size_t last_arg_len=strlen(jm->bg_job_arr[i].args[args_counter-1]);
                if (jm->bg_job_arr[i].args[args_counter-1][last_arg_len-1]=='&') // save arg without &
                   {
                         jm->bg_job_arr[i].args[args_counter-1][last_arg_len-1]=' ';
                        
                    }
            
                jm->jobs_counter++;
                return 1;
            }
        }
    }
    else
    {        
        return 0;
    }
}

void InitJobsManager(jobs_manager *jm)
{
    jm->bg_job_arr=NULL;
    jm->jobs_counter=0;
    for (int i = 0; i < MAX_BG_PROCS; i++)
    {      
        jm->status[i] = false;
    }
}

void InitJob(bg_job *job)
{   
        job->pid=0;
        job->command_line_copy.line_size=0;
        job->command_line_copy.str=NULL;

        for (int j=0 ; j<MAX_ARGS; j++)
        {
           job->args[j]=NULL;
        }
}

void BackgroundJobsPoll(jobs_manager *jm)
{
    for (int i = 0; i < jm->jobs_counter; i++)
    {
        int job_status;
                
        if (jm->status[i]) //if job is still active
        {
            if (waitpid(jm->bg_job_arr[i].pid, &job_status, WNOHANG)>0)
            {
                printf("hw1shell: pid %d finished\n",jm->bg_job_arr[i].pid); 
                ClearJob(jm,i);
            }
            else if (waitpid(jm->bg_job_arr[i].pid, &job_status, WNOHANG)<0)
            {
                printf("hw1shell: %s failed, errno is %d\n","waitpid()",errno);
            }                     
        }
    } 
}


void ClearJob(jobs_manager *jm,int i)
{
    jm->status[i] = 0; 
    jm->bg_job_arr[i].pid=0;
    if (jm->bg_job_arr[i].command_line_copy.str)
    free(jm->bg_job_arr[i].command_line_copy.str);
}


