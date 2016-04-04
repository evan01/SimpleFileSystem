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
static const int NAMESIZE = 16;
static const int EXTENSION_SIZE = 4;
static const int INODE_TABLE_SIZE = 100;

/* Data Structures */
//Inode structure to represent a single file
struct iNode{
    char name[NAMESIZE];
    char extension[EXTENSION_SIZE];
    int mode;
    int size; //Size in blocks of the file
    int blocks[totalBlocks];//Array of all the blocks belonging to the file
    int status;
};

//Superblock structure
struct Super_Block{
    int blockSize;
    int fileSystemSize;
    int iNodeTableLength;
    int freeBlocks[totalBlocks];
    int rootDirPointer;
};

/* In memory data structures */
struct iNode files[INODE_TABLE_SIZE]; //iNode table
struct Super_Block *superBlock; //Super block

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
    if(superBlock->freeBlocks[blk]== 0 || blk == 0 || blk == totalBlocks-1 || blk == 1){
        //Then yes there are free blocks
        superBlock->freeBlocks[blk] = '1';
        write_blocks(blk, nblocks, buffer);
    }else{
        //There are no more free blocks
        printf("Sorry, the block requested B[%d/%d] isn't free\n",blk,totalBlocks);
    }

}

//This function will copy X number of bytes from the first pointer to the next
void copyBytes(int numBytes, void *src, void *dest){
        memcpy(dest,src,numBytes);
}

//This function creates a super block in the first block
void createNewSystem(){
    //Create an in memory superblock structure
    superBlock = createBlock();
    superBlock->blockSize = blockSize;
    superBlock->fileSystemSize = blockSize*totalBlocks;
    superBlock->iNodeTableLength = 0;//No files or directories in superdir to start
    superBlock->rootDirPointer = 0;

    //This function creates a brand new bit map
    for (int i = 0; i < totalBlocks; ++i) {
        superBlock->freeBlocks[i] = 0;
    }
}

//This function reads pre-existing information on the disk and loads the memory data structures
void loadSystemFromDisk(){
    //First load the superblock
    superBlock = createBlock();
    read_blocks(0,1,superBlock);
    
    //Then set up the iNode table
    int iNodeDiskSize = (superBlock->iNodeTableLength)* sizeof(struct iNode);

    //Set up the root directory iNode
    read_blocks(superBlock->rootDirPointer,1,files);
    int a =2;

}

//This function will write the pre-existing data structures to disk
void writeMemToDisk(){
    
    //Update the super block
    writeToDisk(0,1,superBlock);
    
    //Update the iNodeTable
    //Write the entire iNode table to memory if the iNode table is super long
    //Figure out how many blocks the iNode table takes up
    int blocks_iNodeTable = (sizeof(struct iNode)*INODE_TABLE_SIZE)/(superBlock->blockSize);
    writeToDisk(1,blocks_iNodeTable,files);
}


//This function creates the file system
void mksfs(int fresh){
	if (fresh>=1){
		/* Then we initialize a new disk */
		init_fresh_disk(diskName,blockSize,totalBlocks); //Creates the fresh disk of propper size
        createNewSystem();//Creates the super block, sends it to disk
        
	}else{
		//Open an old disk, will use the same disk Name as before...
        init_disk(diskName, blockSize, totalBlocks);
        loadSystemFromDisk();
	}
    //Write the current cached memory to the disk
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


