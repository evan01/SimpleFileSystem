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

//to help address file writing
struct filePointer{
    int block;
    int byte;
};

//Inode structure to represent a single file
struct iNode{
    char name[NAMESIZE];
    char extension[EXTENSION_SIZE];
    int free; //1 for used, and 0 for unused
    int type;//1 for directory, 0 otherwise
    int size; //Size in blocks of the file
    struct filePointer fPointer; //Represents the read/write pointer
    int blocks[TOTAL_BLOCKS];//Array of all the blocks belonging to the file
    char *memBlkPointer;//A pointer to the files contents in memory, byte addressable
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
    int used;
    char fileName[NAMESIZE];
    int iNodeNum;
};

struct Directory{
    struct DirectoryEntry *files;
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
        if (Main_iNodeTable->Table[i].free == 0){
            //There is a free spot to put an iNode into
            Main_iNodeTable->Table[i] = node;
            node.free = 1;
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

int requestFreeBlock(struct iNode *node){
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        if(superBlock->freeBlocks[i]==0) {
            //We have space for the block
            superBlock->freeBlocks[i] = 1;//Mark the block as reserved now
            node->blocks[node->size] = i; //Add the block to the iNode
            node->size++; //Increase the size of the iNode by 1
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
    rootDir.type=1;
    rootDir.free = 1;
    rootDir.size = 0;
    requestFreeBlock(&rootDir); //Request a block on disk to be allocated
    strcpy(rootDir.name,"ROOT");

    //Create directory data structure, No files in it to start
    RootDirectory.numFiles = 0;
    RootDirectory.files = malloc(sizeof(struct DirectoryEntry)*INODE_TABLE_SIZE);

    //Place the directory iNode in the iNode Table
    Main_iNodeTable = malloc(sizeof(struct iNodeTable));
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

struct iNode getInode(int id){
    return Main_iNodeTable->Table[id];
}

/*
	Gets the name of the next file in the directory
*/
int sfs_getnextfilename(char *fname){
    //first increment the file pointer
    static int fileId = 0;
    if (++fileId >100 ){
        fileId  = 0;
    }

    //Search for the file in the directory
    int wrapAround=0;
    while(RootDirectory.files[fileId].used != 1){
        //Don't return the filename of a past deleted file
        fileId++;
        if (++fileId > INODE_TABLE_SIZE){
            if (wrapAround ==1){
                printf("Can't find the file, wrapped around\n");
                return -1;
            }else{
                fileId  = 0;
                wrapAround = 1;
            }

        }
    }

    strcpy(fname, RootDirectory.files[fileId].fileName);
    return fileId;
}

//Returns the iNode number in the iNode table by Name
int findINodeID(char *name){

    int i;
    struct DirectoryEntry dirEntry;
    for (i = 0; i < RootDirectory.numFiles; ++i) {
        dirEntry = RootDirectory.files[i];
        if (strcmp(dirEntry.fileName,name)==0 && dirEntry.used ==1){
            //The file is in the directory
            return RootDirectory.files[i].iNodeNum;
        }
    }
    if (i==RootDirectory.numFiles){
        printf("Sorry can't find the file in the file table\n");
        return -1;
    }
    return -1;
}

//Returns the name of the iNode at a certain ID
char * findINodeName(int id){
    return RootDirectory.files[id].fileName;
}

/*
	Get the size of a file
*/
int sfs_getfilesize(const char* path){
    //First find the iNode for the file in the RootDirectory
    int id = findINodeID(path);
    struct iNode node = getInode(id);
    return node.size;
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
    //newFile.blocks[0]=requestFreeBlock();
    strcpy(newFile.name,fileName);
    newFile.free=1;
    newFile.size = 0;
    newFile.type = 0;

    //Points to the beginning of the file
    newFile.fPointer.block = 0;
    newFile.fPointer.byte = 0;

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

    struct iNode node;
    //First check to see if the file exists.
    int fileID = findINodeID(name);
    if ( fileID < 0){
        //Then the file exists and is in the table, add it to the open files table
        fileID = createNewFile(name); //Does not request a free block to write this file to
    }
    openfileTable[fileID] = 1;
    //Load the files data from the disk into a buffer for writing
    node = getInode(fileID);
    node.memBlkPointer = malloc(node.size*blockSize);
    for (int i = 0; i < node.size; ++i) {
        read_blocks(node.blocks[i],1,node.memBlkPointer);
    }
    return fileID;
}

//this should write all of a files contents to the correct locations on disk
int writeFileToDisk(int fileID){
    //First get the files iNode
    struct iNode node = getInode(fileID);
    //Then for every block the iNode owns write the correct buffer section
    for (int i = 0; i < node.size; ++i) {
        write_blocks(node.blocks[i],1,node.memBlkPointer);
        node.memBlkPointer += blockSize;
    }
    return 0;
}


/*
	Closes given file by id
*/
int sfs_fclose(int fileID){
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        if (openfileTable[fileID]== 1){
            printf("Closing the file with id: %d",fileID);
            openfileTable[fileID] = 0;
            //Write the files contents to disk
            writeFileToDisk(fileID);
        }
    }
    //Frees the memory that file took up
    free(getInode(fileID).memBlkPointer);
    return 0;
}

/*
	write buf characters into disk
*/
int sfs_fwrite(int fileID, char *buf, int length){

    char name[NAMESIZE];
    strcpy(name,findINodeName(fileID));
    sfs_fopen(name);
    struct iNode node = getInode(fileID);

    //Write the whole length of the file into the disk
    int byteswritten = 0;

    for (byteswritten = 0; (byteswritten < length); ++byteswritten){
        node.memBlkPointer[node.fPointer.block*node.fPointer.byte] = buf[byteswritten];

        if (node.fPointer.byte == blockSize-1){
            //We have no more space in this block, at the last byte
            if (node.size < node.fPointer.block){
                //Allocate another block
                //First copy all current data into a buffer of correct size
                char *buffer = malloc((node.size+1)*blockSize);
                copyBytes(blockSize*node.size,node.memBlkPointer,buffer);
                free(node.memBlkPointer);
                //Then allocate the previous files
                node.memBlkPointer = buffer;
                requestFreeBlock(&node);
            }
            //Write to the next block in memory, because we have more blocks to write to
            node.fPointer.block++;//inc file pointer to point to the next block
            node.fPointer.byte = 0;//reset the byte pointer
        }else{
            node.fPointer.byte++;
        }
    };

    //Write the file to the disk
    return writeFileToDisk(fileID);
}

/*
	Read characters from disk into the buffer
*/
int sfs_fread(int fileID, char *buf, int length){
    char name[NAMESIZE];
    strcpy(name,findINodeName(fileID));
    sfs_fopen(name);
    struct iNode node = getInode(fileID);


    //Copy the bytes into the buffer
    copyBytes(length,buf,&node.memBlkPointer);
    return 0;
}
/*
	Moves r/w pointer to a given location
*/
int sfs_fseek(int fileID,int loc){
    struct iNode node = getInode(fileID);
    node.fPointer.block = loc/blockSize;
    node.fPointer.byte = loc%blockSize;
    return 0;
} 

/*
	removes a file from the filesystem
*/
int sfs_remove(char *file){
    int fileID = findINodeID(file);
    struct iNode node = getInode(fileID);
    free(node.memBlkPointer);
    for (int i = 0; i < node.size; ++i) {
        //for every block the iNode owns
        superBlock->freeBlocks[node.blocks[i]] = 0;
    }

    superBlock->iNodeTableLength--;
    superBlock->fileSystemSize--;

    //Now do the directory and the iNodetable
    RootDirectory.numFiles--;
    struct DirectoryEntry dirEntry;
    //Mark the directory slot as being unused
    for (int i = 0; i < RootDirectory.numFiles; ++i) {
        dirEntry = RootDirectory.files[i];
        if (strcmp(dirEntry.fileName,file)==0 && dirEntry.used ==1){
            //Remove the file
            dirEntry.used = 0;
            break;
        }
    }
    //For the iNode table mark the iNode as being free as well
    Main_iNodeTable->Table[fileID].free = 0;


    return 0;
}


