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
static const int TOTAL_BLOCKS = 4;
static const int NAMESIZE = 16;
static const int EXTENSION_SIZE = 4;
static const int INODE_TABLE_SIZE = 100;

/* Data Structures */
//Inode structure to represent a single file
struct iNode{
    char name[NAMESIZE];
    char extension[EXTENSION_SIZE];
    int mode;
    int type;//1 for directory, 0 otherwise
    int size; //Size in blocks of the file
    int blocks[TOTAL_BLOCKS];//Array of all the blocks belonging to the file
    int status;
};

//Superblock structure
struct Super_Block{
    int blockSize;
    int fileSystemSize;
    int iNodeTableLength;
    int freeBlocks[TOTAL_BLOCKS];
    int rootDirPointer; //The number in the iNode table to find the rootDir always 0
};

/* In memory data structures */
struct iNode iNodeTable[INODE_TABLE_SIZE]; //iNode table
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
    if(superBlock->freeBlocks[blk]== 0 || blk == 0 || blk == TOTAL_BLOCKS-1 || blk == 1){
        //Then yes there are free blocks
        superBlock->freeBlocks[blk] = '1';
        write_blocks(blk, nblocks, buffer);
    }else{
        //There are no more free blocks
        printf("Sorry, the block requested B[%d/%d] isn't free\n",blk,TOTAL_BLOCKS);
    }

}

//This function will copy X number of bytes from the first pointer to the next
void copyBytes(int numBytes, void *src, void *dest){
        memcpy(dest,src,numBytes);
}

int requestFreeBlock(){
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        if(superBlock->freeBlocks[i]==0) {
            superBlock->freeBlocks[i] = 1;
            return i;
        }
    }
    printf("ERROR, There are no free blocks\n");
    return -1;//This means that there are no free blocks left
}


//This function creates a super block in the first block
void createNewSystem(){

    //First create a brand new bit map
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        superBlock->freeBlocks[i] = 0;
    }

    //Create an in memory superblock structure
    requestFreeBlock();//This should mark the first block as not free.
    superBlock = createBlock();
    superBlock->blockSize = blockSize;
    superBlock->fileSystemSize = blockSize*TOTAL_BLOCKS;
    superBlock->iNodeTableLength = 1;//Just the root directory for now
    superBlock->rootDirPointer = 0; //The rootDirectory iNode is located at position 0 in the table

    //Set up the iNode table

    //Create the root directory iNode
    struct iNode rootDir;
    rootDir.size=1; //Nothing in the root directory yet
    rootDir.type=1;
    rootDir.blocks[0] = requestFreeBlock(); //Don't bother checking if block is used, marks second block as not
    strcpy(rootDir.name,"ROOT");

    //Place the iNode in the table
    iNodeTable[0]=rootDir;

    //Write the superblock to disk
    writeToDisk(0,1,superBlock);

    //write the iNodeTable to disk
    writeToDisk(1,1,iNodeTable);

}

//This function reads pre-existing information on the disk and loads the memory data structures
void loadSystemFromDisk(){
    //First load the superblock
    superBlock = createBlock();
    read_blocks(0,1,superBlock);
    
    //Then set up the iNode table
    int iNodeDiskSize = (superBlock->iNodeTableLength)* sizeof(struct iNode);

    //Set up the root directory iNode
//    read_blocks(superBlock->rootDirPointer,1,files);

}

//This function creates the file system
void mksfs(int fresh){
	if (fresh>=1){
		/* Then we initialize a new disk */
		init_fresh_disk(diskName,blockSize,TOTAL_BLOCKS); //Creates the fresh disk of propper size
        createNewSystem();//Creates the super block, sends it to disk
        
	}else{
		//Open an old disk, will use the same disk Name as before...
        init_disk(diskName, blockSize, TOTAL_BLOCKS);
        loadSystemFromDisk();
	}
    //Write the current cached memory to the disk
//    writeMemToDisk();
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
int sfs_fclose(int fileID){
    return 0;
}

/*
	write buf characters into disk
*/
int sfs_fwrite(int fileID, char *buf, int length){
    return 0;
}

/*
	Read characters from disk into the buffer
*/
int sfs_fread(int fileID, char *buf, int length){
    return 0;
}
/*
	seek to the location from beginning
*/
int sfs_fseek(int fileID,int loc){
    return 0;
} 

/*
	removes a file from the filesystem
*/
int sfs_remove(char *file){
    
    return 0;
}


