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
//TODO change these constants
static char *diskName = "Evan_Knox_sfs";
static const int blockSize = 1024;
static const int totalBlocks = 4;

/* Super block pointers */
char *superBlock;
int *freeBlocks;//Array of all the free blocks


//TODO create an array of inodes called rootDirectory, this will hold all the file/directory information
//This function will create an in memory block filled with '-'
void* createBlock(){
    char *ptr = malloc(blockSize);
    for (int i = 0; i < blockSize; ++i) {
        ptr[i] = '-';
    }

    return ptr;
}

//This function will handle the writing to the disk and updating of data structures
void writeToDisk(int blk, int nblocks, void *buffer){
    //We can always overwrite the superBlock and the freeBlocks disk blocks
    if(freeBlocks[blk]== 0 || blk == 0 || blk == totalBlocks-1){
        //Then yes there are free blocks
        freeBlocks[blk] = '1';
        write_blocks(blk, nblocks, buffer);
    }else{
        //There are no more free blocks
        printf("Sorry, the block requested B[%d/%d] isn't free\n",blk,totalBlocks);
    }

}

//This function will copy X number of bytes from the first pointer to the next
//Pretty redundant actually given it just calls memcpy
void copyBytes(int numBytes, void *src, void *dest){
        memcpy(dest,src,numBytes);
}

//This function creates a brand new bit map
void createBitMap(int numBlocks){
    //(Memory) Create a virtual array representing the free blocks, it'll have it's own block
    freeBlocks = createBlock();
    for (int i = 0; i < numBlocks; ++i) {
        freeBlocks[i] = '0';
    }
}

//This function creates a super block in the first block
void createSuperBlock(){
    //First create a properly formatted block in memory...
    superBlock = createBlock();

    char magicVal = '7';
    copyBytes(4,&magicVal,superBlock);
    superBlock[13] = 'L';

    superBlock[9] = '6';
    superBlock[10] = '9';

}

//This function reads a pre-existing super block
void readSuperBlock(){
    //TODO implement this method
    
    //First allocate the bit map
}

//This function will write the pre-existing data structures to disk
void writeMemToDisk(){
    
    //Update the super block
    writeToDisk(0,1,superBlock);
    //Update the i-node Table
    
    //Write the bit-map to the disk last
    writeToDisk(totalBlocks-1,1,freeBlocks);
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
		init_fresh_disk(diskName,blockSize,totalBlocks); //Creates the fresh disk of propper size
        createBitMap(totalBlocks);//Creates the bit map block
        createSuperBlock();//Creates the super block, sends it to disk
        
	}else{
		//Open an old disk, will use the same disk Name as before...
        init_disk(diskName, blockSize, totalBlocks);
        readSuperBlock();
        
	}
    //This means that the file pointer fp is now initialized, this represents our disk
    writeMemToDisk();
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


