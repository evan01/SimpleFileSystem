//
//  sfs_test_evans.c
//  SimpleFileSystem
//
//  Created by Evan Knox on 2016-03-27.
//  Copyright © 2016 NaliApplications. All rights reserved.
//

#include <stdio.h>
#include "sfs_api.h"

int main(){
    printf("Initializing the filesystem\n");
    
    mksfs(1);

    system("xxd Evan_Knox_sfs ");
    
}