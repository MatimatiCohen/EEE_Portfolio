#include "filesys.h"

int get_block_idx_in_fs(int block_num_in_dir, dinode *dir_inode, FILE *fp)
{
    //direct block
    if (block_num_in_dir < NDIRECT)
    {
        return (dir_inode->addrs[block_num_in_dir]);
    }
    //indirect block
    else
    {
        int block_location;
        //save the current position to restore later
        int original_file_pos = ftell(fp);
        //seek to indirect block
        assert(!fseek(fp, dir_inode->addrs[NDIRECT] * BSIZE, SEEK_SET));
        //seek to address of block_location
        assert(!fseek(fp, (block_num_in_dir - NDIRECT) * sizeof(int), SEEK_CUR));
        //get block index
        assert(fread(&block_location, sizeof(int), 1, fp) == 1);
        //go back to original position in file
        assert(!fseek(fp, original_file_pos, SEEK_SET));
        return (block_location);
    }
}

void get_inode_size_type(ushort inum, int inodestart, FILE *fp, int *inode_size, short *inode_type)
{
    //save the current position to restore later
    int original_file_pos = ftell(fp);
    dinode cur_inode;
    //goto inode
    goto_inode(inum, inodestart, fp); 
    //parse inode
    assert(fread(&cur_inode, sizeof(dinode), 1, fp) == 1);
    *inode_size = cur_inode.size;
    *inode_type = cur_inode.type;
    //go back to original position in file
    assert(!fseek(fp, original_file_pos, SEEK_SET));
}

void goto_inode(ushort inum, int inodestart, FILE *fp)
{
    //goto inodes
    assert(!fseek(fp, inodestart * BSIZE, SEEK_SET)); 
    //goto inode #inum
    assert(!fseek(fp, sizeof(dinode) * inum, SEEK_CUR));
}

void copy_file(ushort inum, int inodestart, FILE *fp_src, FILE *fp_dst)
{
    dinode file_inode;
    //goto inode
    goto_inode(inum, inodestart, fp_src);
    //parse inode
    assert(fread(&file_inode, sizeof(dinode), 1, fp_src) == 1);

    int num_blocks_in_file  = ceil_div(file_inode.size, BSIZE);
    int num_bytes_in_lst_block = file_inode.size % BSIZE;
    //foreach data block
    for (int block_num_in_file = 0; block_num_in_file < num_blocks_in_file; block_num_in_file++)
    {
        char block_data[BSIZE]; //char is 1-byte long
        int block_idx_in_fs = get_block_idx_in_fs(block_num_in_file, &file_inode, fp_src);
        //goto data block
        assert(!fseek(fp_src, block_idx_in_fs * BSIZE, SEEK_SET));
        //full block
        if ((block_num_in_file < (num_blocks_in_file - 1)) || ((block_num_in_file == (num_blocks_in_file - 1)) && (num_bytes_in_lst_block == 0)))
        {
            //read block data from xv6
            fread(block_data, sizeof(block_data), 1, fp_src);
            //write block data to linux
            fwrite(block_data, sizeof(block_data), 1, fp_dst);
        }
        else //last block is not full
        {
            //read block data from xv6
            fread(block_data, sizeof(char), num_bytes_in_lst_block, fp_src);
            //write block data to linux
            fwrite(block_data, sizeof(char), num_bytes_in_lst_block, fp_dst);
        }
    }
}

bool scan_root_dir(dinode* root_inode, FILE* fs_img_fp, superblock* sb, UserArgs* user_args, int* inum_of_file_to_copy)
{
    int num_blocks_in_root = ceil_div(root_inode->size, BSIZE);
    //go over all blocks in root
    for (int block_num_in_root = 0; block_num_in_root < num_blocks_in_root; block_num_in_root++)
    {
        int block_idx_in_fs = get_block_idx_in_fs(block_num_in_root, root_inode, fs_img_fp);
        //goto root_inode data block
        assert(!fseek(fs_img_fp, block_idx_in_fs * BSIZE, SEEK_SET));
        //go over all dirents in block
        for (int i = 0; i < MAX_DIRENTS_IN_BLOCK; i++)
        {
            dirent cur_dirent;
            int dir_size;
            short dir_type;

            //parse current dirent
            assert(fread(&cur_dirent, sizeof(dirent), 1, fs_img_fp) == 1);

            if (cur_dirent.inum == 0) //inum == 0 marks empty inode, could be a "hole"
            {
                continue;
            }

            //get file syze and type
            get_inode_size_type(cur_dirent.inum, sb->inodestart, fs_img_fp, &dir_size, &dir_type);
            // user_args.command == LS
            if (user_args->command == LS){
                printf("%-*s\t%d %-*d %d\n", DIRSIZ, cur_dirent.name, dir_type, USHORT_MAX_DIGITS, cur_dirent.inum, dir_size);
            }
            // user_args.command == CP
            else
            {
                //if we found the file name and it's type is file - found
                if (!strcmp(cur_dirent.name, user_args->xv6_file) && dir_type == T_FILE)
                {
                    *inum_of_file_to_copy = cur_dirent.inum;
                    return(true);
                }
            }
        }
    }
    return false;
}