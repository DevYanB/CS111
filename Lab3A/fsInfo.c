//NAME: Devyan Biswas
//EMAIL: devyanbiswas@outlook.com
//ID: #UID

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <getopt.h>

#include "ext2_fs.h"

#define SB_OFFSET 1024 //superblock offset value
#define BAD_ARG 1 //return code for bad argument
#define PROC_ERR 2 //return code for processing or corruption error

struct ext2_super_block super_block; //global for the super_block object that will be gotten within main
unsigned int block_size; //block size
int disc_fd = -1; //this is the disc file descriptor that is an integer describing where to start looking


//process the superblock structure passed and, provided it passes some quick checks, print it to stdout
void process_superblock(){
  ssize_t superblock_check = pread(disc_fd, &super_block, sizeof(super_block), SB_OFFSET);
  //check the above returns correctly
  if(superblock_check < 0){
    fprintf(stderr, "Error: problem with reading the superblock\n");
    exit(PROC_ERR);
  }

  //check the magic number(defines the file system) is correct for EXT2
  if(super_block.s_magic != EXT2_SUPER_MAGIC){
    fprintf(stderr, "Error: super_block -- incorrect magic number than expected EXT2 val\n");
    exit(PROC_ERR);
  }

  //check that the log of the block size is reasonable
  if(super_block.s_log_block_size > 64){
    fprintf(stderr, "Error: super_block -- problem with block size passed\n");
    exit(PROC_ERR);
  }
  block_size = SB_OFFSET << super_block.s_log_block_size;

  fprintf(stdout , "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n" , super_block.s_blocks_count, super_block.s_inodes_count,
	  block_size, super_block.s_inode_size, super_block.s_blocks_per_group, super_block.s_inodes_per_group,
	  super_block.s_first_ino);
}


void process_directory(unsigned int inode_num , unsigned int inode_block){
  struct ext2_dir_entry dir;
  unsigned long offset = SB_OFFSET + ((inode_block - 1)*block_size);
  unsigned int bytes = 0;

  while(bytes < block_size){
    memset(dir.name , 0 , 256);
    pread(disc_fd, &dir, sizeof(dir), offset + bytes);
    if(dir.inode != 0){
      memset(&dir.name[dir.name_len], 0, 256 - dir.name_len);
      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",inode_num, bytes, dir.inode, dir.rec_len, dir.name_len, dir.name);
    }
    bytes += dir.rec_len;
  }
}

void time_format(time_t raw_time, char* buf) {
  time_t epoch = raw_time;
  struct tm ts = *gmtime(&epoch);
  strftime(buf, 80, "%m/%d/%y %H:%M:%S", &ts);
}

void process_inode(unsigned int inode_table, unsigned int offset_start, unsigned int inode_num ){
  struct ext2_inode inode;
  unsigned long offset = (offset_start * sizeof(inode)) + (SB_OFFSET + ((inode_table - 1)*block_size));
  ssize_t chk = pread(disc_fd, &inode, sizeof(inode), offset);
  if(chk < 0){
    fprintf(stderr , "Error: problem with reading block when attempting to free\n");
    exit(PROC_ERR);
  }
  if(inode.i_mode == 0 || inode.i_links_count == 0){ //from spec; non-zero link and mode are expected
    return;
  }
  unsigned int num_blocks = (inode.i_blocks / (2 << super_block.s_log_block_size)) * 2; //from online searching

  char file_type = '?';
  //fprintf(stdout, "%d\n" , inode.i_mode); //I use this to figure out that you only need to look at the top value of this to get all the relevant info for this lab regarding the i_mode member
  //u_16 meaning 4 hex chars (16 / 4); only want top 4 bits, so shift by 12
  __u16 top_file_val = (inode.i_mode >> 12) << 12;
  if(top_file_val == 0xa000){
    file_type = 's';
  }
  else if(top_file_val == 0x8000){
    file_type ='f';
  }
  else if(top_file_val == 0x4000){
    file_type ='d';
  }
  //formatting time
  char c_buf[20];
  char m_buf[20];
  char a_buf[20];
  //strftime(c_buf, 40, "%m/%d/%y %H:%M:%S", &m);
  //strftime(m_buf, 40, "%m/%d/%y %H:%M:%S", &f);
  //strftime(a_buf, 40, "%m/%d/%y %H:%M:%S", &g);

  time_format(inode.i_ctime, c_buf);
  time_format(inode.i_mtime, m_buf);
  time_format(inode.i_atime, a_buf);
  //0xfff below becasue want lower 12 bits
  fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d", inode_num, file_type, inode.i_mode & 0xfff, inode.i_uid, inode.i_gid, inode.i_links_count, c_buf, m_buf, a_buf, inode.i_size, num_blocks);

  //printing out block addr's
  unsigned int r;
  for (r = 0; r < 15; r++) {
    fprintf(stdout, ",%d", inode.i_block[r]);
  }
  fprintf(stdout, "\n");

  //processing directory entries
  for(r = 0; r < 12; r++){
    if(inode.i_block[r] != 0 && file_type == 'd'){
      process_directory(inode_num , inode.i_block[r]);
    }
  }

  //managing indirect inodes
  if (inode.i_block[12] != 0) {
    __u32* bl_ptrs = malloc(block_size);
    __u32 ptrs = block_size / sizeof(__u32);

    unsigned long indir_1_offset = SB_OFFSET + ((inode.i_block[12] - 1)*block_size);
    pread(disc_fd, bl_ptrs, block_size, indir_1_offset);

    unsigned int i;
    for (i = 0; i < ptrs; i++) {
      if (bl_ptrs[i] != 0) {
	if (file_type == 'd') {
	  process_directory(inode_num, bl_ptrs[i]);
	}
	fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,1,12 + i,inode.i_block[12],bl_ptrs[i]);
      }
    }
    free(bl_ptrs);
  }

  //managing doubly indirect inodes
  if (inode.i_block[13] != 0) {
    __u32* ind_bl_ptrs = malloc(block_size);
    __u32 num_ptrs = block_size / sizeof(__u32);

    unsigned long indir_2_offset = SB_OFFSET + ((inode.i_block[13] - 1) * block_size);
    pread(disc_fd, ind_bl_ptrs, block_size, indir_2_offset);

    unsigned int k;
    unsigned int log_offset_st = 256 + 12;
    for (k = 0; k < num_ptrs; k++) {
      if (ind_bl_ptrs[k] != 0) {
	fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,2,log_offset_st + k,inode.i_block[13],ind_bl_ptrs[k]);

	//search through this indirect block to find its directory entries
	__u32* block_ptrs = malloc(block_size);
	unsigned long indir_offset = SB_OFFSET + ((ind_bl_ptrs[k] - 1) * block_size);
	pread(disc_fd, block_ptrs, block_size, indir_offset);

	unsigned int e;
	for (e = 0; e < num_ptrs; e++) {
	  if (block_ptrs[e] != 0) {
	    if (file_type == 'd') {
	      process_directory(inode_num, block_ptrs[e]);
	    }
	    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,1,log_offset_st + e,ind_bl_ptrs[k],block_ptrs[e]);
	  }
	}
	free(block_ptrs);
      }
    }
    free(ind_bl_ptrs);
  }

  //managing triply indirect inodes
  if (inode.i_block[14] != 0) {
    __u32* ind2_blck_pntrs = malloc(block_size);
    __u32 num_ptrs = block_size / sizeof(__u32);

    unsigned long indir_3_offset = SB_OFFSET + ((inode.i_block[14] - 1) * block_size);
    pread(disc_fd, ind2_blck_pntrs, block_size, indir_3_offset);

    unsigned int j;
    unsigned int log_offset_st = 65536 + 256 + 12;
    for (j = 0; j < num_ptrs; j++) {
      if (ind2_blck_pntrs[j] != 0) {
	fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,3,log_offset_st + j,inode.i_block[14],ind2_blck_pntrs[j]);

	__u32* ind_blck_pntrs = malloc(block_size);
	unsigned long indir_2_offset = SB_OFFSET + ((ind2_blck_pntrs[j] - 1) * block_size);
	pread(disc_fd, ind_blck_pntrs, block_size, indir_2_offset);

	unsigned int k;
	for (k = 0; k < num_ptrs; k++) {
	  if (ind_blck_pntrs[k] != 0) {
	    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,2,log_offset_st + k,ind2_blck_pntrs[j], ind_blck_pntrs[k]);
	    __u32* block_ptrs = malloc(block_size);
	    unsigned long indir_offset = SB_OFFSET + ((ind_blck_pntrs[k] - 1) * block_size);
	    pread(disc_fd, block_ptrs, block_size, indir_offset);

	    unsigned int l;
	    for (l = 0; l < num_ptrs; l++) {
	      if (block_ptrs[l] != 0) {
		if (file_type == 'd') {
		  process_directory(inode_num, block_ptrs[l]);
		}
		fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",inode_num,1,log_offset_st + l,ind_blck_pntrs[k],block_ptrs[l]);
	      }
	    }
	    free(block_ptrs);
	  }
	}
	free(ind_blck_pntrs);
      }
    }
    free(ind2_blck_pntrs);
  }
}


//this function is similar to the one below checking free block entries
//however, in addition, if the inode's bitmap value is 1, then you need to process_inode
void check_inode_entries(int group_num, unsigned int inode_block_num, unsigned int inode_table){
  int bytes_inode = super_block.s_inodes_per_group / 8;
  char* bytes_to_free = (char*) malloc(bytes_inode);
  unsigned long inodes_offset = SB_OFFSET + ((inode_block_num - 1)*block_size);
  unsigned int tracker = (super_block.s_inodes_per_group * group_num) + 1;
  //fprintf(stdout , "%d\n" , tracker);
  unsigned int start = tracker;

  ssize_t chk = pread(disc_fd, bytes_to_free, bytes_inode, inodes_offset);
  if(chk < 0){
    fprintf(stderr , "Error: problem with reading block when attempting to free\n");
    exit(PROC_ERR);
  }
  int i;
  int j;

  for(i = 0; i < bytes_inode; i++){
    char t = bytes_to_free[i];
    for(j = 0; j < 8; j++){
      int status_used = t & 1;
      if(!status_used){
	fprintf(stdout, "IFREE,%d\n" , tracker);
      }
      else if(status_used){
	process_inode(inode_table, tracker - start, tracker);
      }
      t >>= 1;
      tracker++;
    }
  }
  free(bytes_to_free);
}


void free_block_entries(int group_num , unsigned int block_num){
  //remember, in the bitmap of block entries in the fs img, 0 is free and 1 is used
  char* bytes_to_free = (char*) malloc(block_size);
  unsigned long block_offset = SB_OFFSET + ((block_num - 1)*block_size); //equation is from the offset of the blocks starting with super block plus the number of blocks till you get to the one ya want to remove
  unsigned int tracker = super_block.s_first_data_block +(super_block.s_blocks_per_group * group_num); //just gets you to the data block you want to start removal with, using the inbuilt member of first data block of super block to do this
  ssize_t chk = pread(disc_fd,bytes_to_free, block_size, block_offset);
  if(chk < 0){
    fprintf(stderr , "rror: problem with reading block when attempting to free\n");
    exit(PROC_ERR);
  }
  unsigned int i;
  unsigned int j;
  for(i = 0;i < block_size; i++){ //outer thing is an array of block_size values
    char t = bytes_to_free[i];
    for(j = 0;j < 8; j++){ //each one is 8 long
      int status_used = t & 1;
      if(!status_used){
	fprintf(stdout , "BFREE,%d\n" , tracker);
      }
      t >>= 1;
      tracker++;
    }
  }
  free(bytes_to_free);
}


//get the group info; located right after superblock info in fs img
void get_group_info(int group_num, int t_groups){
  struct ext2_group_desc g_desc;
  __u32 description_block = 0;
  if(block_size == 1024){
    description_block = 2;
  }
  else{
    description_block = 1;
  }

  unsigned long offset_group = description_block*block_size + 32 * group_num;
  ssize_t group_check = pread(disc_fd, &g_desc, sizeof(g_desc), offset_group);
  if(group_check < 0){
    fprintf(stderr, "Error: problem with reading group descriptor block\n");
    exit(PROC_ERR);
  }

  unsigned int blocks_in_group = super_block.s_blocks_per_group; //given quantity in superblock structure
  if(group_num == t_groups-1){
    blocks_in_group = super_block.s_blocks_count - (super_block.s_blocks_per_group * (t_groups - 1)); //this is to compensate for potential overflow issues
  }

  unsigned int inodes_in_group = super_block.s_inodes_per_group; //given quantity in superblock structure
  if(group_num == t_groups-1){
    inodes_in_group = super_block.s_inodes_count - (super_block.s_inodes_per_group * (t_groups - 1));
  }

  fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n" , group_num, blocks_in_group, inodes_in_group, g_desc.bg_free_blocks_count, g_desc.bg_free_inodes_count, g_desc.bg_block_bitmap, g_desc.bg_inode_bitmap, g_desc.bg_inode_table);

  //to free the block entries, need the bitmap
  unsigned int block_bitmap = g_desc.bg_block_bitmap;
  free_block_entries(group_num , block_bitmap);

  unsigned int inode_bitmap = g_desc.bg_inode_bitmap;
  unsigned int inode_table = g_desc.bg_inode_table;
  check_inode_entries(group_num, inode_bitmap, inode_table);
}


int main(int argc, char* argv[]) {

  struct option opts[] = {
    {0,0,0,0}
  };
  //argument number check
  if (argc != 2) {
    fprintf(stderr, "Bad arguments\n");
    exit(BAD_ARG);
  }
  //getting argument check
  if (getopt_long(argc, argv, "", opts, NULL) != -1) {
    fprintf(stderr, "Bad arguments\n");
    exit(BAD_ARG);
  }
  //opening file system image check
  if((disc_fd = open(argv[1],O_RDONLY)) == -1){
    fprintf(stderr, "Bad arguments\n");
    exit(BAD_ARG);
  }
  //process the superblock in the passed file system image
  process_superblock();
  //getting number of blocks using this to account for lower val
  int groups_num = 1 +  ((super_block.s_blocks_count-1) / super_block.s_blocks_per_group);
  // fprintf(stdout , "%d\n", groups_num);
  int i = 0;
  //go through each one; again, not really needed, since my spec promised only one block group,  but better safe than sorry.
  for(; i < groups_num; i++){
    get_group_info(i , groups_num);
  }
  return 0;
}
