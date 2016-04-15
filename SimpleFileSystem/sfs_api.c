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
static const int MAX_OPEN_FILES = 100;

/* Data Structures */
//Inode structure to represent a single file
struct iNode{
    char name[NAMESIZE];
    char extension[EXTENSION_SIZE];
    int mode; //1 for used, and 0 for unused
    int type;//1 for directory, 0 otherwise
    int size; //Size in blocks of the file
    int blocks[TOTAL_BLOCKS];//Array of all the blocks belonging to the file
    char *memBlkPointer; //Points to an IN MEMORY block to write to
};

struct iNodeTable{
    struct iNode Table[INODE_TABLE_SIZE]; //iNode table
    int numiNodes;
};

//Superblock structure
struct Super_Block{
    int blockSize;
    int fileSystemSize;
    int iNodeTableLength;
    int freeBlocks[TOTAL_BLOCKS];
    int rootDirPointer; //The number in the iNode table to find the rootDir always 0
};

struct DirectoryEntry{
    char fileName[NAMESIZE];
    int iNodeNum;
};

struct Directory{
    struct DirectoryEntry *files;
    int selectedFile;
    int numFiles;
};


/* In memory data structures */

struct Super_Block *superBlock; //Super block
struct Directory RootDirectory; //holds the names and locations of iNodes in the iNode table
int openfileTable[MAX_OPEN_FILES]; //Holds the numbers of iNodes that are currently open
struct iNodeTable *Main_iNodeTable; //Holds all of the INodes

//This function will create an in memory block filled with '-'
void* createBlock(){
    char *ptr = malloc(blockSize);
    for (int i = 0; i < blockSize; ++i) {
        ptr[i] = '-';
    }

    return ptr;
}

int addNodeToTable(struct iNode node){

    for (int i = 0; i < INODE_TABLE_SIZE; ++i) {
        if (Main_iNodeTable->Table[i].mode == 0){
            //There is a free spot to put an iNode into
            Main_iNodeTable->Table[i] = node;
            Main_iNodeTable->numiNodes++;
            return 1;
        }
    }
    printf("Unable to add iNode to table, table is full\n");
    return -1;
};

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

    //Create an in memory superblock structure
    superBlock = createBlock();

    //First create a brand new bit map
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        superBlock->freeBlocks[i] = 0;
    }
    superBlock->blockSize = blockSize;
    superBlock->fileSystemSize = blockSize*TOTAL_BLOCKS;
    superBlock->iNodeTableLength = 1;//Just the root directory for now
    superBlock->rootDirPointer = 0; //The rootDirectory iNode is located at position 0 in the table
    superBlock->freeBlocks[0] = 1;//Mark the first block as being used

    //Set up the iNode table
    //Create the root directory iNode
    struct iNode rootDir;
    rootDir.size=1; //Nothing in the root directory yet
    rootDir.type=1;
    rootDir.blocks[0] = requestFreeBlock(); //Request a block on disk to be allocated
    strcpy(rootDir.name,"ROOT");

    //Create directory data structure, No files in it to start
    RootDirectory.numFiles = 0;
    RootDirectory.selectedFile = 0;
    RootDirectory.files = malloc(sizeof(struct DirectoryEntry)*INODE_TABLE_SIZE);

    //Place the directory iNode in the iNode Table
    addNodeToTable(rootDir);

    //Write the superblock to disk
    writeToDisk(0,1,superBlock);

    //write the iNodeTable to disk, for now it's just 1 block
    writeToDisk(1,1,Main_iNodeTable);

}

//This function reads pre-existing information on the disk and loads the memory data structures
void loadSystemFromDisk(){
    //First load the superblock
    superBlock = createBlock();
    read_blocks(0,1,superBlock);
    
    //Then set up the iNode table
  //  int iNodeDiskSize = (superBlock->iNodeTableLength)* sizeof(struct iNode);

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
    //first increment the file pointer
    RootDirectory.selectedFile = RootDirectory.selectedFile + 1;
    if (RootDirectory.numFiles > RootDirectory.selectedFile){
        //Then return the name of the file, by placing the name in the buffer
        strcpy(fname, RootDirectory.files[RootDirectory.selectedFile].fileName);
        return 1;
    }else{
        return -1;
    }
}

//Returns the iNode number in the iNode table by Name
int findINodeNumByName(char *name){

    int i;
    for (i = 0; i < RootDirectory.numFiles; ++i) {
        if (strcmp(RootDirectory.files[i].fileName,name)==0){
            //The file is in the directory
            return RootDirectory.files[i].iNodeNum;
        }
    }
    if (i==RootDirectory.numFiles){
        printf("Sorry can't find the file in the file table\n");
        return -1;
    }

}

/*
	Get the size of a file
*/
int sfs_getfilesize(const char* path){
    //First find the iNode for the file in the RootDirectory
    int iNodeNum = findINodeNumByName(path);
    return fileINode.size;
}

/*
 * This function will
 *  1. Assign a block on disk for the file
 *  2. Create a Directory entry for the file
 *  3. Create an entry in the iNodeTable for the file
 *
 */
int createNewFile(char *fileName){

    //first create the iNode, reserve a block for the files data
    struct iNode newFile;
    newFile.blocks[0]=requestFreeBlock();
    strcpy(newFile.name,fileName);
    newFile.mode=1;
    newFile.size = 1;
    newFile.type = 0;

    //Place the iNode in the inode table
    int iNodeId = addNodeToTable(newFile);

    //Then add this to the directory data structure
    if (iNodeId < 0){
        printf("Error, too many files in the directory can't create another");
        return  -1;
    }else{
        RootDirectory.numFiles++;
        RootDirectory.files[RootDirectory.numFiles].fileName;
        //Make sure that the dir entry points to the propper iNode
        RootDirectory.files[RootDirectory.numFiles].iNodeNum = iNodeId;
    }
    return iNodeId; //Returns the id of the iNode in the iNode table

}


/*
	Creates/opens a file with the name given
*/
int sfs_fopen(char *name){
    //First check to see if the file is open or not.
    int fileID = findINodeNumByName(name);
    if ( fileID > 0){
        //Then the file exists and is in the table, add it to the open files table
        openfileTable[fileID] = 1; //Mark this file as open
        //Load the files data from the disk
        struct iNode node = Main_iNodeTable->Table[fileID];
        read_blocks()
    }else{
        //Create a new file
        fileID = createNewFile(name);
        openfileTable[fileID] = 1;
    }
    return 0;
}


//This will take whatever was in the open files current buffer and write that block to disk
int writeFileToDisk(){

}

/*
	Closes given file by id
*/
int sfs_fclose(int fileID){
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        if (openfileTable[fileID]== 1){
            printf("Closing the file with id: %d",fileID);
            writeFileToDisk();
        }
    }
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


