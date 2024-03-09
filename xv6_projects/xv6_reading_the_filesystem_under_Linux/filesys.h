#ifndef FILESYS_H
#define FILESYS_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

// structures & defines taken from xv6-public/fs.h
//////////////////////////
#define BSIZE 512  // block size

typedef struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
} superblock;

#define ROOTINO 1  // root i-number
#define NDIRECT 12

#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
typedef struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
} dinode;

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

typedef struct dirent {
  ushort inum;
  char name[DIRSIZ];
} dirent;
//////////////////////////

#define MAX_DIRENTS_IN_BLOCK (BSIZE / sizeof(dirent))

//functions decleration
int get_block_idx_in_fs(int block_num_in_dir, dinode *dir_inode, FILE *fp);
void get_inode_size_type(ushort inum, int inodestart, FILE *fp, int *inode_size, short *inode_type);
void goto_inode(ushort inum, int inodestart, FILE *fp);
void copy_file(ushort inum, int inodestart, FILE *fp_src, FILE *fp_dst);
bool scan_root_dir(dinode* root_inode, FILE* fs_img_fp, superblock* sb, UserArgs* user_args, int* inum_of_file_to_copy);

#endif