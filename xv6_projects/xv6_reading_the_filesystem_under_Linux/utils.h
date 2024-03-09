#ifndef UTILS_H
#define UTILS_H

#include <string.h>

#define FS_IMG_ARG 1
#define COMMAND_ARG 2
#define XV6_FILE_ARG 3
#define LINUX_FILE_ARG 4
#define USHORT_MAX_DIGITS 5

//enums
enum command {LS, CP};

//structures
struct UserArgs{
    int command;
    char *fs_img_file;
    char *xv6_file;
    char *linux_file;
};
typedef struct UserArgs UserArgs;

//functions decleration
int GetArgs(UserArgs *user_args, int argc, char** args);
int ceil_div(int num, int den); //returns ceil(num/den)

#endif