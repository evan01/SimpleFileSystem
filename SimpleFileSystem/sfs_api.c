//
//  sfs_api.c
//  SimpleFileSystem
//
//  This file contains the source code implementation of the file system
//

#include "sfs_api.h"


/* Constants */
static char *diskName = "simple_disk";
static const int blockSize = 100;
static const int numBlocks = 30;


/* Structs that represent file information */



/*
	This function creates the file system
*/
void mksfs(int fresh){
	if (fresh>1){
		/* Then we initialize a new disk */
		init_fresh_disk(diskName,blockSize,numBlocks);
	}else{
		//Open an old disk, will use the same disk Name as before...
        init_disk(diskName, blockSize, numBlocks);
	}
    //This means that the file pointer fp is now initialized, this represents our disk
}


/*
	Gets the name of the next file in the directory
*/
int sfs_getnextfilename(char *fname){

    return 0;
}

/*
	Get the size of a file
*/
int sfs_getfilesize(const char* path){

    return 0;
}

/*
	Opens a file with the name given
*/
int sfs_fopen(char *name){

    return 0;
}

/*
	Closes given file by id
*/
void sfs_fclose(int fileID){

}

/*
	write buf characters into disk
*/
void sfs_fwrite(int fileID, char *buf, int length){

}

/*
	Read characters from disk into the buffer
*/
void sfs_fread(int fileID, char *buf, int length){

}
/*
	seek to the location from beginning
*/
void sfs_fseek(int fileID,int loc){

} 

/*
	removes a file from the filesystem
*/
int sfs_remove(char *file){
    
    return 0;
}


