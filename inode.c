//FUNCTIONS TO PERFORM SPECIFIC OPERATIONS ON THE INODE STRUCTURES.
#include "header.h"



//initialize the inodes in the  inode table
int inode_init() {
	for (int i = 0; i < MAX_NUM_INODE ; i++)
	 {
		for (int j = 0; j < MAX_BLK_PER_FILE; j++) 
		{
			//for each inode setting the data-blocks it points to  to -1 that means that the inode is free and does not contain any data
			inode_table.inode[i].block_id_list[j] = -1;
		}
	}
	//writing the address to starting address of the inode table to a file
	void *tmp_it = malloc(BLOCK_SIZE);
	memcpy(tmp_it, &inode_table, sizeof(inode_table));
	write_block_at(1, tmp_it);
	free(tmp_it);
	
	return 0;
}

//to get the inode number which is free
int get_inode() 
{
	//allocating memeory to store the starting address of the inode table
	void *tmp_it = malloc(BLOCK_SIZE);
	//getting the address of the inode table
	read_block(1, tmp_it);
	//writing the address to the inode talbe  structure
	memcpy(&inode_table, tmp_it, sizeof(inode_table));
	//freeing up the temp variable
	free(tmp_it);

	//serching for free inode
	//int free_inode,
	int flag = 0;
	for(int i = 0; i < MAX_NUM_INODE ; i++) {
		//for each inode in the inode table is the data-block content is empty that is it
		//does not point to any data-block that means the inode is free
		if(inode_table.inode[i].block_id_list[0]==-1) 
		{
			flag=1;
			//return the inode position which is free
			return i;
		}
	}
	//if all inode is occupied returuning -1
	if(flag==0)
		return -1;
	else{return -1;}
}


//freeing up the inode
int free_inode(int inode_num) 
{
	/*if(inode_num > MAX_NUM_INODE  - 1 || inode_num < 0) {
		return -1;
	}*/
	
		//setting the data-blocks to -1 indicating that it does not point to any data-blocks
		for (int i = 0; i < MAX_BLK_PER_FILE; i++) {
			inode_table.inode[inode_num].block_id_list[i]=-1;
		
	}
	return 0;
}

//to get the block that the current inode reffers to
int get_block(int inode_num) 
{
	//loading the address of the inode table
	void *tmp_it = malloc(BLOCK_SIZE);
	read_block(1, tmp_it);
	memcpy(&inode_table, tmp_it, sizeof(inode_table));
	free(tmp_it);	
	//-1 means free, otherwies occupied
	return inode_table.inode[inode_num].block_id_list[0];
}

//to get the number of links 
int get_nlink(int inode_num) 
{
	//getting the address of the inode table
	void *tmp_it = malloc(BLOCK_SIZE);
	read_block(1, tmp_it);
	memcpy(&inode_table, tmp_it, sizeof(inode_table));
	free(tmp_it);

	/*if(inode_num > MAX_NUM_INODE  - 1 || inode_num < 0) {
		return 1;
	}*/
	return inode_table.inode[inode_num].nlink;
}

//to get the field of a perticular inode
size_t get_size(int inode_num)
 {
	//loading the inode table
	void *tmp_it = malloc(BLOCK_SIZE);
	read_block(1, tmp_it);
	memcpy(&inode_table, tmp_it, sizeof(inode_table));
	free(tmp_it);	
	//returning the size 
	return inode_table.inode[inode_num].sz;
}

//to update or set the link fild of a inode in the inode table
int set_nlink(int inode_num , int value) 
{
	
	inode_table.inode[inode_num].nlink=value;
	
	void *tmp_it = malloc(BLOCK_SIZE);
	memcpy(tmp_it, &inode_table, sizeof(inode_table));
	//updating the address of the inode table
	write_block_at(1, tmp_it);
	free(tmp_it);
	
	return 0;
}

//updating the block list of a inode when it points to some data block 
int set_block(int inode_num , int block_id) 
{
	
	//updating the block_list of a perticular inode given by inode number
	inode_table.inode[inode_num].block_id_list[0]=block_id;
	//updating the inode table entries
	void *tmp_it = malloc(BLOCK_SIZE);
	memcpy(tmp_it, &inode_table, sizeof(inode_table));
	write_block_at(1, tmp_it);
	free(tmp_it);
	
	return 0;
}

//to set the size of a inode 
int set_size(int inode_num, size_t sz) 
{
	
	//setting the size
	inode_table.inode[inode_num].sz = sz;
	//updating the inode talbe address that is the meta-data
	void *tmp_it = malloc(BLOCK_SIZE);
	memcpy(tmp_it, &inode_table, sizeof(inode_table));
	write_block_at(1, tmp_it);
	free(tmp_it);
	
	return 0;
}

//to fill the directory entries
int fill_dir_ent(struct dir_block_t *dir_block, char *entry, enum type_t type) 
{
	int i;
	for (i = 0; i < MAX_NUM_DIRENT; ++i) {
		// empty entry => free
		//by defafult if a single member of the directory is initialized then the remaining directories will be initialized to
		// its default values.
		if (!(strlen(dir_block->dir_ent[i].name))) {
			break;
		}
	}
	if (i >= MAX_NUM_DIRENT) {
		printf("not enough directory entries available\n");
		return 1;
	}
	strcpy(dir_block->dir_ent[i].name, entry);
	dir_block->dir_ent[i].type = type;
	
	if (type == REG_FILE) {
		dir_block->dir_ent[i].inode_id = get_inode();
		printf("inode id: %d\n", dir_block->dir_ent[i].inode_id);
		inode_table.inode[dir_block->dir_ent[i].inode_id].mode=S_IFREG|0777;
		set_nlink(dir_block->dir_ent[i].inode_id, 1);	
	}
	
	return 0;
}
