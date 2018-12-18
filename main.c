/*	gcc -Wall sfs9.c `pkg-config fuse --cflags --libs` -o sfs 	*/

#include "header.h"



 void init_init() 
{
	printf("init-inti\n");
	int free_directory_index = 0;
	strcpy(dir.dir_block[free_directory_index].path, "/");
	fill_dir_ent(&(dir.dir_block[free_directory_index]), ".", DIR);
	fill_dir_ent(&(dir.dir_block[free_directory_index]), "..", DIR);
	dir.dir_block[free_directory_index].num_ent = 2;

	d_init_init();

	void *tmp_dir = malloc(BLOCK_SIZE);
	memcpy(tmp_dir, &dir, sizeof(dir));
	write_block_at(0, tmp_dir);
	free(tmp_dir);

	inode_init();
}

 void *sfs_init(struct fuse_conn_info *conn) 
{	printf("here init\n");
	(void) conn;
	
	int fp = open("./meta_data", O_RDWR,0777);
	if(fp==-1)
	{	
		init_init();
		int fp = open("./meta_data", O_RDWR|O_CREAT,0777);
		if(fp<1)
		{
			printf("FILE DOES NOT EXISTS/ MOUNTING FOR THE FIRST TIE !!!!\n");
		}
		printf("creating files and root directory\n");
	}
	else
	{
		init_disk();

		// read directory from disk
		
		void *tmp_dir = malloc(BLOCK_SIZE);
		//loading the root directory
		read_block(0, tmp_dir);
		memcpy(&dir, tmp_dir, sizeof(dir));
		free(tmp_dir);
	
		
	}
	
	// initializing fd_table
	for (int i = 0; i < MAX_NUM_FDT; i++) {
		fd_table.file_desc[i].fd = -1;
		fd_table.file_desc[i].inode_id = -1;
	}
	//close(fp);
	
	return NULL;
}

 int sfs_getattr(const char *path, struct stat *stbuf) 
{
	printf("getattr\n");
	
	
	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));
	
	if (!strcmp(path, "/")) {
		//int directory_index = 0;
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		stbuf->st_uid = getuid();
		stbuf->st_gid = getgid();
		stbuf->st_atime=time(NULL);
    	stbuf->st_mtime=time(NULL);
    	stbuf->st_ctime=time(NULL);
		return 0;
	}
	
	// find parent in directory
	int parent_path_index;
	for (parent_path_index = 0; parent_path_index < MAX_DIR_LEN; ++parent_path_index) {
		if (!(strcmp(par_path, dir.dir_block[parent_path_index].path))) {
			break;
		}	
	}
	
	// path does not exist
	if (parent_path_index >= MAX_DIR_LEN) {
		return -ENOENT;
	}
	
	/*if (name == NULL || par_path == NULL) {
		return -ENOENT;
	}*/
	
	// find entry in parent whether the current file exists in this path or not
	int j;
	for (j = 0; j < MAX_NUM_DIRENT; ++j) {
		if (!(strcmp(name, dir.dir_block[parent_path_index].dir_ent[j].name))) {
			break;
		}
	}

	// file or directory does not exist
	if (j >= MAX_NUM_DIRENT) {
		return -ENOENT;
	}

	// determine type, file or directory
	if (dir.dir_block[parent_path_index].dir_ent[j].type == REG_FILE) {
		printf("inod =%d\n",dir.dir_block[parent_path_index].dir_ent[j].inode_id);
		stbuf->st_mode = S_IFREG | inode_table.inode[dir.dir_block[parent_path_index].dir_ent[j].inode_id].mode;
		stbuf->st_nlink = get_nlink(dir.dir_block[parent_path_index].dir_ent[j].inode_id);
		stbuf->st_size = get_size(dir.dir_block[parent_path_index].dir_ent[j].inode_id);
		stbuf->st_ino = dir.dir_block[parent_path_index].dir_ent[j].inode_id;
		stbuf->st_blksize = BLOCK_SIZE;
		stbuf->st_blocks  = 1;
		stbuf->st_uid = getuid();
		stbuf->st_gid = getgid();
		stbuf->st_atime=time(NULL);
    	stbuf->st_mtime=time(NULL);
    	stbuf->st_ctime=time(NULL);
	} else {
		int directory_index;
		for (directory_index = 0; directory_index < MAX_DIR_LEN; ++directory_index) {
			if (!strcmp(path, dir.dir_block[directory_index].path)) {
				break;
			}
		}
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		stbuf->st_blksize=BLOCK_SIZE;
		stbuf->st_size=BLOCK_SIZE;
		stbuf->st_blocks=1;
		stbuf->st_uid = getuid();
		stbuf->st_gid = getgid();
	  	stbuf->st_atime=time(NULL);
    	stbuf->st_mtime=time(NULL);
    	stbuf->st_ctime=time(NULL);
	}

	return 0;
	
}


 int sfs_chmod(const char *path, mode_t setPerm) {
    

	printf("chmod\n");
	
	
	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));


	if (!strcmp(path, "/")) {
		//cannot change the directory permisions
		return -EACCES;
	}
	// find parent in directory
	int parent_path_index;
	for (parent_path_index = 0; parent_path_index < MAX_DIR_LEN; ++parent_path_index) {
		if (!(strcmp(par_path, dir.dir_block[parent_path_index].path))) {
			break;
		}	
	}
	
	// path does not exist
	if (parent_path_index >= MAX_DIR_LEN) {
		return -ENOENT;
	}
	
	/*if (name == NULL || par_path == NULL) {
		return -ENOENT;
	}*/
	
	// find entry in parent whether the current file exists in this path or not
	int j;
	for (j = 0; j < MAX_NUM_DIRENT; ++j) {
		if (!(strcmp(name, dir.dir_block[parent_path_index].dir_ent[j].name))) {
			break;
		}
	}

	// file or directory does not exist
	if (j >= MAX_NUM_DIRENT) {
		return -ENOENT;
	}

	// determine type, file or directory
	if (dir.dir_block[parent_path_index].dir_ent[j].type == REG_FILE) {
		printf("inod =%d\n",dir.dir_block[parent_path_index].dir_ent[j].inode_id);		
		printf("current mode : %lo(ocatal)\n",(unsigned long)inode_table.inode[dir.dir_block[parent_path_index].dir_ent[j].inode_id].mode);
		//printf("new mode : %lo(ocatal)\n",setPerm);
		inode_table.inode[dir.dir_block[parent_path_index].dir_ent[j].inode_id].mode=setPerm;
		printf("new mode : %lo(ocatal)\n",(unsigned long)inode_table.inode[dir.dir_block[parent_path_index].dir_ent[j].inode_id].mode);

		void *tmp_node = malloc(BLOCK_SIZE);
		memcpy(tmp_node, &inode_table, sizeof(inode_table));
		write_block_at(1, tmp_node);
		free(tmp_node);
		return 0;
		
		
	}
	else if(dir.dir_block[parent_path_index].dir_ent[j].type == DIR)
	{
		return -EACCES;
	}

	else
		return 0;
}

 int sfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) 
{
	//printf("here\n");
	
	printf("readdir\n");
	// read directory from disk
	// read_block(0, &dir);
	
	int flag = 0;
	for(int i = 0; i < MAX_DIR_LEN; i++) {
		if(strcmp(path, dir.dir_block[i].path) == 0) {
			flag = 1;
			
			for (int j = 0; j < MAX_NUM_DIRENT; j++) {
				if(strlen(dir.dir_block[i].dir_ent[j].name)) {
					filler(buf, dir.dir_block[i].dir_ent[j].name, NULL, 0);
				}

			}
			
			break;
		}
	}
	// if no dir with given path is there
	if(!flag) {
		printf("here\n");
		return -1;
	}

	return 0;
}

 int sfs_mkdir(const char *path, mode_t mode) 
{
	//printf("here\n");
	
	printf("mkdir\n");
	// read directory from disk
	// read_block(0, &dir);

	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));

	int free_directory_index ;
	for (free_directory_index = 0; free_directory_index < MAX_DIR_LEN; ++free_directory_index) {
		// empty path => free entry
		if (!strlen(dir.dir_block[free_directory_index].path)) {
			break;
		}
	}

	if (free_directory_index >= MAX_DIR_LEN) {
		printf("not enough space\n");
		return 1;
	}
	
	// add path
	strcpy(dir.dir_block[free_directory_index].path, path);
	dir.dir_block[free_directory_index].num_ent = 2;

	// initialise directory with '.' and '..'
	fill_dir_ent(&(dir.dir_block[free_directory_index]), ".", DIR);
	fill_dir_ent(&(dir.dir_block[free_directory_index]), "..", DIR);
	
	// find parent directory
	int parent_path_index;
	for (parent_path_index = 0; parent_path_index < MAX_DIR_LEN; ++parent_path_index) {
		if (!(strcmp(par_path, dir.dir_block[parent_path_index].path))) {
			break;
		}	
	}
	
	printf("parent_path_index: %d\n", parent_path_index);

	// add entry to parent directory
	fill_dir_ent(&(dir.dir_block[parent_path_index]), name, DIR);

	// increase hardlink count
	dir.dir_block[parent_path_index].num_ent++;
	
	// wirte directory
	void *tmp_dir = malloc(BLOCK_SIZE);
	memcpy(tmp_dir, &dir, sizeof(dir));
	write_block_at(0, tmp_dir);
	free(tmp_dir);
	
	free(name);
	free(par_path);
	free(dup_path);
	
	return 0;
}


 int sfs_rmdir(const char *path) 
{	//printf("here\n");

	printf("rmdir\n");
	// read directory from disk
	// read_block(0, &dir);

	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));

	// find required path
	int i;
	for (i = 0; i < MAX_DIR_LEN; ++i) {
		if (!strcmp(path, dir.dir_block[i].path)) {
			break;
		}		
	}
	
	// path does not exist
	if (i == MAX_DIR_LEN) {
		return -ENOENT;
	}
	
	if (dir.dir_block[i].num_ent > 2) {
		printf("Directory is not empty \n");
		return -ENOTEMPTY;
	}
	
	// make path empty
	dir.dir_block[i].path[0] = '\0';
	dir.dir_block[i].num_ent = 0;
	
	// find parent in directory
	for (i = 0; i < MAX_DIR_LEN; ++i) {
		if (!strcmp(par_path, dir.dir_block[i].path)) {
			break;
		}	
	}
	
	// find entry in parent
	int j;
	for (j = 0; j < MAX_NUM_DIRENT; ++j) {
		if (!strcmp(name, dir.dir_block[i].dir_ent[j].name)) {
			break;
		}
	}
	
	// make entry empty
	dir.dir_block[i].dir_ent[j].name[0] = '\0';
	dir.dir_block[i].num_ent--;
	
	// wirte directory
	void *tmp_dir = malloc(BLOCK_SIZE);
	memcpy(tmp_dir, &dir, sizeof(dir));
	write_block_at(0, tmp_dir);
	free(tmp_dir);
	
	free(name);
	free(par_path);
	free(dup_path);
	
	return 0;
}

 int sfs_open(const char *path, struct fuse_file_info *fi) 
{//printf("here2\n");
	
	
printf("open\n");
	int flags=fi->flags;

	int flag1=0;
	int flag2=0;
	//int flag3=0;
	int fd, myInode;

	char *dup_path = strdup(path);
	char *file_name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));
	
	//Finding inode of file
	for (int i = 0; i < MAX_DIR_LEN ; i++) {
		//searching for the parent path directory
		if (strcmp (par_path , dir.dir_block[i].path) == 0) 
		{
			flag1 = 1;
			//searching for the file in this directory
			for (int j = 0; j < MAX_NUM_DIRENT ; j++) 
			{
				if(strcmp (file_name,dir.dir_block[i].dir_ent[j].name) == 0) 
				{
					myInode = dir.dir_block[i].dir_ent[j].inode_id;
					printf("inode of the file %d\n",myInode);
				}
			}
		}
	}
	
	//Finding smallest free fd
	for (int i = 0 ; i < MAX_NUM_FDT ; i++) 
	{
		if(fd_table.file_desc[i].fd == -1) 
		{
			fd_table.file_desc[i].fd = i;
			fd_table.file_desc[i].inode_id = myInode;
			flag2 = 1;
			fd = i;
			printf("assigning smallest file descriptor\n");
			break;	
		}
	}

	if(flag1  && flag2) 
	{	//if the file is to be opened for append mode set the curretn offset to be the size of the file.
		if (flags & O_APPEND) 
		{
			printf("opening append mode\n");
			fd_table.file_desc[fd].current_off = get_size(myInode);
		}
		printf("returning file descriptor\n");
		return fd;
	}
	return -1;
}

 int sfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) 
{//printf("here3\n");
	
	printf("%s\n",path);
	int fildes=fi->fh;
	printf("read\n");
	int inode_id, block, off;
	char *b = malloc(BLOCK_SIZE);
	//checking whether the file trying to be read exists in the file descriptor  table or not
	/*if(fd_table.file_desc[fildes].inode_id != -1) {*/
		//gettign the inode value of that file so that contents can be read
		inode_id = fd_table.file_desc[fildes].inode_id;


		//getting the current off set
		off = fd_table.file_desc[fildes].current_off;

		//reading the datablock id that the inode refers to
		block = get_block(inode_id);
		printf("block id=%d\n",block);
		printf("here\n");
		//reading the datablock content that  inode points to and coping it to the buffer variable b
		if(read_block(block, b) == 0)
		{	
			//checcking for the amount of data present after the offset value 
			//checking whether the amount of data present is less than the size or not
			int i = 0;
			while ((b + off)[i] != EOF && i < size) 
			{
				++i;
			}
			printf("size: %d\n", i);
			int nb = (i < size) ? i : size;
			memcpy(buf, b + off, nb);
			//setting the offset value
			fd_table.file_desc[fildes].current_off += nb;
			printf("read2\n");
			return nb;
		}
		return -1;
	
}


 int sfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) 
{
	//printf("here\n");
	

	printf("write \n");
	
	int fildes=fi->fh;
	
	int inode_id,block, off;
	//checking whether the file is present in the file tablel
	if(fd_table.file_desc[fildes].inode_id != -1)
	{
		inode_id = fd_table.file_desc[fildes].inode_id;
		off = fd_table.file_desc[fildes].current_off;
		
		char *tmp_data = malloc(BLOCK_SIZE);
		
		// first read already written stuff then write at appropriate position
		if ((block = get_block(inode_id)) != -1) {
			printf("file already exists, offset: %ld, off: %d\n", offset, off);
			read_block(block, tmp_data);
			if (offset + size > get_size(inode_id)) {
				tmp_data[offset + size] = EOF;
			}
			memcpy(tmp_data + offset, buf, size);
			write_block_at(block, tmp_data);
		} 
		// if file does not exist already
		else {
			//printf("file does not exist, offset: %ld, off: %d\n", offset, off);
			memcpy(tmp_data, buf, size);
			tmp_data[size] = EOF;
			//find a free block write content to that block then inode with that block

			block = write_block(tmp_data);
			if(block == -1) {
				return EPERM;
			}
			set_block(inode_id, block);
		}
		free(tmp_data);
		
		// updating file size.
		printf("current size: %lu\n", get_size(inode_id));
		if (size + offset > get_size(inode_id)) {
			set_size(inode_id, size + offset);
			printf("updated size: %lu\n", get_size(inode_id));			
		}
		
		return size;
	}
	
	else{
		return EBADF;
	}

}

 int sfs_mknod(const char *path, mode_t mode, dev_t dev) 
{
	//printf("mknode\n");
	printf("MK NODE\n");
	// read directory from disk
	// read_block(0, &dir);
	
	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));
	
	// find parent directory
	int parent_path_index;
	for (parent_path_index = 0; parent_path_index < MAX_DIR_LEN; ++parent_path_index) {
		if (!(strcmp(par_path, dir.dir_block[parent_path_index].path))) {
			break;
		}	
	}

	// add entry to parent directory
	fill_dir_ent(&(dir.dir_block[parent_path_index]), name, REG_FILE);

	// increase number link count to the parent directory
	dir.dir_block[parent_path_index].num_ent++;
	
	// wirte directory
	void *tmp_dir = malloc(BLOCK_SIZE);
	memcpy(tmp_dir, &dir, sizeof(dir));
	write_block_at(0, tmp_dir);
	free(tmp_dir);
	
	free(name);
	free(par_path);
	free(dup_path);
		
	return 0;
}


 int sfs_unlink(const char *path) 
{	//printf("unlink\n");
	
	printf("UNLINK\n");

	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));
	
	// find parent in directory
	int i;
	for (i = 0; i < MAX_DIR_LEN; ++i) {
		if (!strcmp(par_path, dir.dir_block[i].path)) {
			break;
		}	
	}
	
	// find entry in parent
	int j;
	for (j = 0; j < MAX_NUM_DIRENT; ++j) {
		if (!strcmp(name, dir.dir_block[i].dir_ent[j].name)) {
			break;
		}
	}
	
	// file does not exist
	if (j > MAX_NUM_DIRENT) {
		return -ENOENT;
	}
	
	// make entry empty
	dir.dir_block[i].dir_ent[j].name[0] = '\0';
	dir.dir_block[i].num_ent--;
	free_inode(dir.dir_block[i].dir_ent[j].inode_id);
	
	// wirte directory
	void *tmp_dir = malloc(BLOCK_SIZE);
	memcpy(tmp_dir, &dir, sizeof(dir));
	write_block_at(0, tmp_dir);
	free(tmp_dir);
	
	free(name);
	free(par_path);
	free(dup_path);
	
	return 0;
}

 int sfs_release(const char *path, struct fuse_file_info *fi) 
{//printf("close\n");
	
	printf("close\n");
	int fd=fi->fh;
	// if file isclear already closed 
	if(fd_table.file_desc[fd].fd==-1) {
		return -1;
		//printf("here\n");
	}
	else {
		//printf("here\n");
		fd_table.file_desc[fd].inode_id = -1;
		fd_table.file_desc[fd].current_off = 0;
		fd_table.file_desc[fd].fd = -1;
	}
	return 0;
}


 int sfs_truncate(const char* path, off_t size)
{

 	printf("TRUNCATE\n");

	char *dup_path = strdup(path);
	char *name = strdup(basename(dup_path));
	char *par_path = strdup(dirname(dup_path));
	
	// find parent in directory
	int i;
	for (i = 0; i < MAX_DIR_LEN; ++i) {
		if (!strcmp(par_path, dir.dir_block[i].path)) {
			break;
		}	
	}
	
	// find entry in parent
	int j;
	for (j = 0; j < MAX_NUM_DIRENT; ++j) {
		if (!strcmp(name, dir.dir_block[i].dir_ent[j].name)) {
			break;
		}
	}

	// file does not exist
	if (j > MAX_NUM_DIRENT) {
		return -ENOENT;
	}
	int inode_id=dir.dir_block[i].dir_ent[j].inode_id;
	size_t len=inode_table.inode[inode_id].sz;
	printf("size=%lu\n",len);
	if(len<size)
	{	char *temp=malloc(BLOCK_SIZE);
		memset(temp,0,size+1);
		temp[size]=EOF;

		//printf("cleared blcok %d\n",clear_block(inode_table.inode[inode_id].block_id_list[0]));
		write_block_at(inode_table.inode[inode_id].block_id_list[0],temp);
		free(temp);
	}
	else
	{
		memset(block[inode_table.inode[inode_id].block_id_list[0]]+len,0,size-len);
	}
	inode_table.inode[inode_id].sz=size;
	return 0;
	    
}


 int sfs_rename(const char *from, const char *to) 
{
	char *dup_path_from = strdup(from);
	char *name_from= strdup(basename(dup_path_from));
	char *par_path_from= strdup(dirname(dup_path_from));
	
	// find parent in directory
	int from_i;
	for (from_i = 0; from_i < MAX_DIR_LEN; ++from_i) {
		if (!strcmp(par_path_from, dir.dir_block[from_i].path)) {
			break;
		}	
	}
	
	// find entry in parent
	int from_j;
	for (from_j = 0; from_j < MAX_NUM_DIRENT; ++from_j) {
		if (!strcmp(name_from, dir.dir_block[from_i].dir_ent[from_j].name)) {
			break;
		}
	}
	
	// file does not exist
	if (from_j > MAX_NUM_DIRENT) {
		return -ENOENT;
	}



	char *dup_path_to= strdup(to);
	char *name_to= strdup(basename(dup_path_to));
	char *par_path_to= strdup(dirname(dup_path_to));
	
	// find parent in directory
	int to_i;
	for (to_i = 0; to_i < MAX_DIR_LEN; ++to_i) {
		if (!strcmp(par_path_to, dir.dir_block[to_i].path)) {
			break;
		}	
	}
	
	// find entry in parent
	int to_j;
	for (to_j = 0; to_j < MAX_NUM_DIRENT; ++to_j) {
		if (!strcmp(name_to, dir.dir_block[to_i].dir_ent[to_j].name)) {
			break;
		}
	}
	
	// if file exists 
	if (to_j <MAX_NUM_DIRENT){

		if( dir.dir_block[from_i].dir_ent[from_j].type==REG_FILE && dir.dir_block[to_i].dir_ent[to_j].type==REG_FILE){
			//overwrite the destination file
			dir.dir_block[to_i].dir_ent[to_j].inode_id=dir.dir_block[from_i].dir_ent[from_j].inode_id;
			strcpy(dir.dir_block[to_i].dir_ent[to_j].name,name_to);
			//clearing the the file from fromm-path	
			dir.dir_block[from_i].dir_ent[from_j].name[0] = '\0';
			dir.dir_block[from_i].num_ent--;

			void *tmp_dir = malloc(BLOCK_SIZE);
			memcpy(tmp_dir, &dir, sizeof(dir));
			write_block_at(0, tmp_dir);
			free(tmp_dir);

			return 0;

		}
		else{
			return -EISDIR;
		}

	}

	else{
		strcpy(dir.dir_block[to_i].dir_ent[dir.dir_block[to_i].num_ent+1].name,name_to);
		dir.dir_block[to_i].dir_ent[dir.dir_block[to_i].num_ent+1]=dir.dir_block[from_i].dir_ent[from_j];
		dir.dir_block[to_i].num_ent++;

		dir.dir_block[from_i].dir_ent[from_j].name[0] = '\0';
		dir.dir_block[from_i].num_ent--;


		void *tmp_dir = malloc(BLOCK_SIZE);
		memcpy(tmp_dir, &dir, sizeof(dir));
		write_block_at(0, tmp_dir);
		free(tmp_dir);

		return 0;
	}
	
}

 int sfs_utimens(const char *path,const struct timespec ts[2]){
	return 0;
}

 struct fuse_operations sfs_oper = {
	.init		= sfs_init,
	.getattr	= sfs_getattr,
	.mkdir		= sfs_mkdir,
	.mknod		= sfs_mknod,
	.readdir	= sfs_readdir,
	.open		= sfs_open,
	.read		= sfs_read,
	.write		= sfs_write,
	.release	= sfs_release,
	.rmdir		= sfs_rmdir,
	.unlink	= sfs_unlink,
	.chmod=sfs_chmod,
	.truncate=sfs_truncate,
	.utimens=sfs_utimens,
	.rename=sfs_rename,
};

int main(int argc, char *argv[]) {
	printf("---------MOUNTING----------!\n");
	return fuse_main(argc, argv, &sfs_oper, NULL);	
}
