//
//  sfs_api.h
//  SimpleFileSystem
//
//  Created by Evan Knox on 2016-03-27.
//  Copyright © 2016 NaliApplications. All rights reserved.
//

#ifndef sfs_api_h
#define sfs_api_h

#include <stdio.h>
#include "disk_emu.h"

#endif /* sfs_api_h */

void mksfs(int fresh);// creates the file system
int sfs_getnextfilename(char *fname); // get the name of the next file in directory
int sfs_getfilesize(const char* path);  // get the size of the given file
int sfs_fopen(char *name);// opens the given file
void sfs_fclose(int fileID); // closes the given file
void sfs_fwrite(int fileID, char *buf, int length);// write buf characters into disk
void sfs_fread(int fileID, char *buf, int length);// read characters from disk into buf
void sfs_fseek(int fileID,int loc);// seek to the location from beginning
int sfs_remove(char *file);// removes a file from the filesystem







