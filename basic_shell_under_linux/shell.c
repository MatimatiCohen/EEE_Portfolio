
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include "jobs.h"
#include "utils.h"

//functions implementation
void PrintDir()
{
    printf("hw1shell$ ");
}

int GetArgs(char* line, char** args)
{
    int i = 0; 

    args[i]=strtok(line," ");
    while (args[i] != NULL)
    {
       
        i++;
        args[i] = strtok(NULL," ");
    }
    if (i>0){
    if (args[i-1][strlen(args[i-1])-1]=='\n'){
    TruncateLastChar(args[i-1]);  
    } 
    }
    args[i] = NULL; //last array argument needs to be null for future execvp work correctly
    return i; //return how many args found
}


int ExecCommand(char** args)
{    
    int execvp_status=execvp(args[0], args);
    if (execvp_status< 0)
    {
        printf("hw1shell: invalid command\n");
        printf("\nhw1shell: %s failed, errno is %d\n","execvp()",errno);        
    }
    return execvp_status;
}

bool InternalCommand(char **args , jobs_manager *jm, int num_of_args)
{
    if (!strcmp(args[0],"cd"))
    {
        if (num_of_args > 2) //num_of_args includes the "cd" command
        {
            printf("hw1shell: invalid command\n");
        }
        else if (chdir(args[1])==-1)
        {
            printf("\nhw1shell: %s failed, errno is %d\n","chdir()",errno);
        }
        return true;
    }
        
    else if (!strcmp(args[0],"jobs"))
    {                
        if (jm->jobs_counter!=0)
        {        
            PrintJobs(jm->bg_job_arr,jm->jobs_counter);        
        }
        return true;
    }
    return false;
}

void ExternalCommand(char **args, jobs_manager *jm, line_data * line)
{
    int wstatus;
    int background_flag=IsBackground(args);
    pid_t pid;
    pid=fork();
    if (pid<0)
    {
        printf("hw1shell: %s failed, errno is %d\n","fork()",errno);
    }
    else if (pid == 0) //child
    {
        int exec_stat=ExecCommand(args);

        exit(0); //if child didn't kill itself, need to be killed
    }
    else //parent
    {
        if (!background_flag) //foreground
        {
            if (waitpid(pid,&wstatus,0)<0)
            {
                printf("hw1shell: %s failed, errno is %d\n","waitpid()",errno);
            }
        }
        else //background
        {  
            undo_strtok(line);         
            if (!UpdateJobs(jm,pid,args,line))
            {
                printf("too many background commands\n");
                
            }
            else
            {
                printf("hw1shell: pid %d started\n",pid); 
            
            }
        }                
    }
}

int arg_count(char** args)
{
    int counter=0;
    int i=0;
    while (args[i])
    {
        i++;
        counter++;
    }

    return counter;
}

void ExitShell(jobs_manager *jm,line_data *command)
{
    if (command->str!=NULL)
    {
        free(command->str);
    }
    if (jm->bg_job_arr>0)
    {
        while (wait(NULL)>0); //wait all the child processes possibly running in the background
        for (int j=0; j<jm->jobs_counter;j++)
        {
            if (jm->status[j]){
             if (jm->bg_job_arr[j].command_line_copy.str!=NULL){
                free(jm->bg_job_arr[j].command_line_copy.str);
             }
            }
        }
        exit(0);
    }
    else
    {
        exit(0);    
    }   
}


