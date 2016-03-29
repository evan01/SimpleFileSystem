//
//  sfs_api.c
//  SimpleFileSystem
//
//  This file contains the source code implementation of the file system
//  You can always use xxd to read the disk
//

#include "sfs_api.h"


/* 
 Constants, only used when creating a brand new file system, to be replaced
 with possible */
static char *diskName = "Evan_Knox_sfs";
static const int blockSize = 1024;
static const int numBlocks = 4;


/* Super block pointers */


//This function will create an in memory block filled with '-'
void* createBlock(){
    char *ptr = malloc(blockSize);
    for (int i = 0; i < blockSize; ++i) {
        ptr[i] = '-';
    }

    return ptr;
}

//This function will copy X number of bytes from the first pointer to the next
void copyBytes(int numBytes, void *src, void *dest){

        memcpy(dest,src,numBytes);
}

//This function creates a super block in the first block
void createSuperBlock(){
    //First create a properly formatted block in memory...
    char *buffer = createBlock();

    //The First 4 bytes will be the Magic field
//    int magicVal = 1234;
    char magicVal = '7';
    copyBytes(4,&magicVal,buffer);

    //The next 4 the Block Size

    //The next 4 the file system size

    //Then
    buffer[9] = '6';
    buffer[10] = '9';

    write_blocks(0, 1, buffer);
//    printf("%d\n",);
}

//This function reads a pre-existing super block
void readSuperBlock(){
    
}


/*
    REQUIRED LIBRARY FUNCTIONS, USED BY TEST FILES
*/

/*
	This function creates the file system
*/
void mksfs(int fresh){
	if (fresh>=1){
		/* Then we initialize a new disk */
		init_fresh_disk(diskName,blockSize,numBlocks);
        createSuperBlock();
	}else{
		//Open an old disk, will use the same disk Name as before...
        init_disk(diskName, blockSize, numBlocks);
        readSuperBlock();
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
    printf("TESTING!!!! PASSS\n");
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


