#include "utils.h"

int GetArgs(UserArgs *user_args,int argc, char** args)
{
    if (argc < 3) return 0; //not enough arguments
    user_args->fs_img_file = args[FS_IMG_ARG];
    if (!strcmp(args[COMMAND_ARG], "ls")) 
    {
        user_args->command = LS;
    }
    else if (!strcmp(args[COMMAND_ARG], "cp"))
    {
        if (argc < 5) return 0; //not enough arguments
        user_args->command = CP;
        user_args->xv6_file = args[XV6_FILE_ARG];
        user_args->linux_file = args[LINUX_FILE_ARG];
    }
    else return 0; //unknown command
    return 1;
}

int ceil_div(int num, int den)
{
    return ((num + (den-1))/den);
}