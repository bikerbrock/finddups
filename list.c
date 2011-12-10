#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
/*  Also, if we find any symbolic links, devices, or other non-regular
 *  file types we will remove those nodes from our list.
 */
int addDirectoryContent(const char* dir, struct Node* previous)
{
//    printf("inside addDirectoryContent\n");
//    printf("dir=%s\n", dir);
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir(dir)) == NULL) {
        printf("Error(%d) opening the directory \"%s\"\n", errno, dir);
//        printf("EACCES %d EMFILE %d ENFILE %d ENOENT %d ENOMEM %d ENOTDIR %d\n", EACCES, EMFILE, ENFILE, ENOENT, ENOMEM, ENOTDIR);
        
        //cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    struct Node* node;
    while ((dirp = readdir(dp)) != NULL) {
        if(strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            /*  Create a new node with the file name and link it to
             *  previous node */
            node = malloc(sizeof(struct Node));
            node->filename = malloc(strlen(dir) + 5 + strlen(dirp->d_name));
            /*  Here we must copy the relative path into our node that we
             *  just created */
            strcpy(node->filename, dir);      // Copy the directory name
            strcat(node->filename, "/");            // Append a slash 
            strcat(node->filename, dirp->d_name);   // Append the file name

//            printf("***ADDING: %s\n", node->filename);// Debugging 

            /*  Link the previous node to the node we just created */
            node->next = previous->next;
            previous->next = node;
            previous = node;
        }
    }
    closedir(dp);
    return 0;
}

