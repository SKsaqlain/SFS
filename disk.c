#include "header.h"
//functions to operate on the meta disk file which acts like a virtual hard disk.

//function called when mounted for the first time
int d_init_init() {
	printf("allocating memeroy to data blocks and bitmaps\n");
	block = malloc(MAX_BLOCK * sizeof(char*));
	bitmap = malloc(MAX_BLOCK * sizeof(int));
	
	
	int i;
	for (i = 0; i < MAX_BLOCK; ++i)
	 {
		block[i] = malloc(BLOCK_SIZE * sizeof(char));
		
	}

	int j;
	for (j = 0; j < MAX_BLOCK; ++j)
	 {
		bitmap[j] = 1;
	}
	
	return 0;
}

int init_disk() 
{
	block = malloc(MAX_BLOCK * sizeof(char*));	//allocating memeroy for the datablocks
	bitmap = malloc(MAX_BLOCK * sizeof(int));	////allocating memeroy for the datanode bitmap
	
	//if fail to allocate memory return -1
	

	//allocating memeory to each datablock in  the datablock list
	int i;
	for (i = 0; i < MAX_BLOCK; ++i)
	 {
		block[i] = malloc(BLOCK_SIZE * sizeof(char));
		
	}
	
	//restoring previous closed session that is block details and bitmap details

	int fp = open("./meta_data", O_RDWR,0777);
	for (i = 0; i < MAX_BLOCK; ++i) {
		read(fp,block[i],BLOCK_SIZE);	
	}
	//close(fp);
	
	
	read(fp,bitmap, sizeof(int)*MAX_BLOCK);
	close(fp);
	
	
	return 0;
}

// read block, given block id
int read_block(int block_id, void *buf)
{
	if (block_id < MAX_BLOCK) 
	{
		memcpy(buf, block[block_id], BLOCK_SIZE);
		//o->successful copy of datat
		return 0;
	}
	//requested data block does not exists
	return -1;
}
//write data to a available free block and return the block id that the data was written on
int write_block(const void *buf) {
	int block_id;
	
	//0 reserved for super block and 1 reserved for inode
	//checking for free datablocks bitmap->0 occupied bitamp->1 means free
	for (block_id = 2; block_id < MAX_BLOCK; ++block_id) 
	{
		if (bitmap[block_id]) 
		{
			break;
		}
	}

	if (block_id < MAX_BLOCK) 
	{
		//coping the data into the datablock
		memcpy(block[block_id], buf, BLOCK_SIZE);
		//setting the bitmap->0 indicating occupied
		bitmap[block_id] = 0;
		//updating contents to the disk file which contains datablock contents
		int fp = open("./meta_data",O_WRONLY,0777);
		int i;
		for (i = 0; i < MAX_BLOCK; ++i)
		 {
			 write(fp,block[i],BLOCK_SIZE);
			//printf("data written at block %d\n",n);
		}
		//close(fp);
		//updating bitmap meta-data file
		
		write(fp,bitmap, sizeof(int)*MAX_BLOCK);
		//printf("data written at block %d\n",n);
		close(fp);
		//returning the id of the block to which it was written
		return block_id;
	}
	//unsuccessful write return -1
	return -1;
}

// write data to a particular block id
int write_block_at(int block_id, const void *buf)
 {
	//if the block_id exceeds the max # of blocks return error -1
	if (block_id < MAX_BLOCK)
	 {
		//overwriting the memeory contents of that block
		memcpy(block[block_id], buf, BLOCK_SIZE);
		//setting bitmap to occupied
		bitmap[block_id] = 0;
		//updating the meta-files
		int fp = open("./meta_data",O_WRONLY,0777);
		int i;
		
		for (i = 0; i < MAX_BLOCK; ++i) 
		{
			 write(fp,block[i],BLOCK_SIZE);
			//printf("data written at block %d\n",n);
		}
		//close(fp);
		
	 	write(fp,bitmap, sizeof(int)*MAX_BLOCK);
		//printf("data written into block %d\n",n);
		close(fp);
		
		return 0;
	}
	
	return -1;
}

//clear the contents of a block
int clear_block(int block_id) 
{
	//just setting the bitmap to 0 indicating free eventhouf the data is present in that data block as the memory contents are overwritten 
	if (block_id >= 0 && block_id < MAX_BLOCK) 
	{
		bitmap[block_id] = 1;
		//updating the meta-files
		int fp = open("./meta_data",O_WRONLY,0777);
		lseek(fp,MAX_BLOCK*BLOCK_SIZE,SEEK_SET);
		write(fp,bitmap, sizeof(int)*MAX_BLOCK);
		close(fp);
		
		return 0;
	}
	
	return -1;
}