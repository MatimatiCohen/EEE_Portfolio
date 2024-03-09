#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "filesys.h"
#include <stdbool.h>

//Holds user arguments
UserArgs user_args;

int main(int argc, char **argv)
{
    FILE *fs_img_fp, *xv6_fp, *linux_fp;
    bool found_file_to_copy = false;
    int inum_of_file_to_copy;
    superblock sb;
    dinode root_inode;

    //get user arguments
    if (!GetArgs(&user_args, argc, argv))
	{
		exit(1);
	}
    
    fs_img_fp=fopen(user_args.fs_img_file,"rb");
    assert(fs_img_fp!=NULL);

    //goto superblock
    assert(!fseek(fs_img_fp, BSIZE, SEEK_SET));
    //parse superblock
    assert(fread(&sb, sizeof(superblock), 1, fs_img_fp) == 1);

    //goto inode #1 (root)
    goto_inode(ROOTINO, sb.inodestart, fs_img_fp);
    //parse inode #1 (root)
    assert(fread(&root_inode, sizeof(dinode), 1, fs_img_fp) == 1);

    //scan root directory
    found_file_to_copy = scan_root_dir(&root_inode, fs_img_fp, &sb, &user_args, &inum_of_file_to_copy);
 
    //copy file from xv-6 to linux
    if (user_args.command == CP)
    {
        if (!found_file_to_copy)
        {
            printf("File %s does not exist in the root directory\n", user_args.xv6_file);
            exit(0);
        }

        linux_fp=fopen(user_args.linux_file,"wb");
        assert(linux_fp!=NULL);

        copy_file(inum_of_file_to_copy, sb.inodestart, fs_img_fp, linux_fp);

        fclose(linux_fp);
    }

    fclose(fs_img_fp);

    exit(0);
}