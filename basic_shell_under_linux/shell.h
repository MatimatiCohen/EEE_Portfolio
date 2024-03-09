#ifndef SHELL_H
#define SHELL_H


#include <stdbool.h>

#include "shell.h"

//functions deceleration
void PrintDir();
int GetArgs(char* line, char** args);
int ExecCommand(char **args );
bool InternalCommand(char **args , jobs_manager *jm, int num_of_args);
void ExternalCommand(char **args, jobs_manager *jm,line_data* line);
int arg_count(char** args);
void ExitShell(jobs_manager *jm,line_data *command);

#endif
