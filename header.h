#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include<time.h>

//SET OF ALL STRUCTURES USED.

#define MAX_NUM_DIRENT 5	//maximum number of directories that a given directory can have
#define MAX_NAME_LEN 20	//name of file or folder
#define MAX_BLK_PER_FILE 5	//maximum number of blocks that a file can reffer
#define MAX_PATH_LEN 20			
#define MAX_DIR_LEN 20		//PATH LENGTH
#define MAX_NUM_INODE  20		//inodet bitmap or maximum number of inodes
#define MAX_NUM_FDT 5		//maximum number of files that can be opened
#define MAX_BLOCK 40	//number of blocks
#define BLOCK_SIZE 4096	//size of eacch block


//DIR=>0
//REG_FILE=>1
enum type_t 
{
	DIR, REG_FILE
};


struct inode_t 
{
	int block_id_list[MAX_BLK_PER_FILE];
	mode_t mode;
	nlink_t nlink;
	size_t sz;

};

struct dir_ent_t 
{
	char name[MAX_NAME_LEN];
	enum type_t type;
	int inode_id;
};

struct dir_block_t 
{
	char path[MAX_PATH_LEN];
	struct dir_ent_t dir_ent[MAX_NUM_DIRENT];
	int num_ent;
};

struct directory_t 
{
	struct dir_block_t dir_block[MAX_DIR_LEN];
};

struct inode_table_t
 {
	struct inode_t inode[MAX_NUM_INODE ];
};

struct file_desc_t 
{
	int fd;
	int inode_id;
	int current_off;
};

struct fd_table_t
 {
	struct file_desc_t file_desc[MAX_NUM_FDT];
};


char **block;	//holds the datablock array
int *bitmap;	//holds the bitmap array

//variable that contains list of inodes
struct inode_table_t inode_table ;


 struct directory_t dir;	//directory tree
 struct fd_table_t fd_table; //file table


//THE CORRESPONDING FUNCTIONS IMPLEMENTATION ARE PRESETN IN inode.c FILE.
int inode_init();
int get_inode();
int free_inode(int inode_num);
int get_block(int inode_num);
int get_nlink(int inode_num);
size_t get_size(int inode_num);
int set_nlink(int inode_num , int value);
int set_block(int inode_num , int block_id);
int set_size(int inode_num, size_t sz);
int fill_dir_ent(struct dir_block_t *dir_block, char *entry, enum type_t type); 


//THE CORRESPONDING FUNCTIONS IMPLEMENTATION ARE PRESENT IN disk.c FILE.
int d_init_init();
int init_disk();
int read_block(int block_id, void *buf);
int write_block(const void *buf);
int write_block_at(int block_id, const void *buf);
int clear_block(int block_id);



//THE CORRESPONDING FUCTIONS  IMPLEMENTATION ARE PRESETN IN main.c FILE.
 void init_init();
 void *sfs_init(struct fuse_conn_info *conn);
 int sfs_getattr(const char *path, struct stat *stbuf);
 int sfs_chmod(const char *path, mode_t setPerm);
 int sfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
 int sfs_mkdir(const char *path, mode_t mode);
 int sfs_rmdir(const char *path);
 int sfs_open(const char *path, struct fuse_file_info *fi);
 int sfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
 int sfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
 int sfs_mknod(const char *path, mode_t mode, dev_t dev);
 int sfs_unlink(const char *path);
 int sfs_release(const char *path, struct fuse_file_info *fi);
 int sfs_truncate(const char* path, off_t size);
 int sfs_rename(const char *from, const char *to);
 int sfs_utimens(const char *path,const struct timespec ts[2]); 