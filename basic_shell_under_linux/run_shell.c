#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <errno.h>
#include "jobs.h"
#include "utils.h"
#include "shell.h"
//constants
#define MAX_BG_PROCS 4
#define MAX_ARGS 64 //according to Gadi's response in the forum

//TODO:  Fix memmory leackages declared by valgrind debugger - strtok is problematic, need to find a solution

//main
int main()
{
    //global variables
    bg_job bg_jobs[MAX_BG_PROCS];
    for (int i=0; i<MAX_BG_PROCS;i++)
    {
     InitJob(&bg_jobs[i]);
    }
    jobs_manager jm;
    InitJobsManager(&jm);
    jm.bg_job_arr=bg_jobs;
    
    while (true)
    {      
        size_t n=0;
        line_data command;
        init_line(&command);
        char *args[MAX_ARGS];
        PrintDir(); //prints new shell line
        getline(&command.str,&n,stdin);
        command.line_size=sizeof(command.str);
 
        int num_of_args = GetArgs(command.str,args);
        BackgroundJobsPoll(&jm);   //a poll to check if background processes are done and update their status. 

        //process new command
        if (!strcmp(args[0],"exit")) //exit command
        { 
            ExitShell(&jm,&command);
        }

        else if (!strcmp(args[0],"\0")) //not a real command, only spaces and \n
        {
                  continue; 
        }
        else if (InternalCommand(args,&jm, num_of_args)) //internal command - jobs or cd    
        {         
        }
        else //external command
        {                        
            ExternalCommand(args,&jm,&command); //any other command
        }

    
        if (command.str!=NULL)
        {
            free(command.str);
        }        
    }
    return 0;
}

