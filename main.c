#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "list.h"

/*  Return value:
 *  a) Positive value which is the file size
 *  b) -1 if there was a problem accessing the file
 */
int getFileSize(const char* path)
{
    struct stat buf;
    if( stat(path, &buf) == 0 )
    {
        return (int)buf.st_size;
    }
        return -1;
}
/*  Return value:
 *  0 - The file is non-regular
 *  1 - The file is regular
 *  -1 - There was a problem reading the file
 */
int isRegular(const char* path)
{
    struct stat buf;
    if( stat(path, &buf) == 0 )
    {
        if( S_ISSOCK( buf.st_mode ) ||  // Is this file a socket? 
            S_ISLNK( buf.st_mode )  ||  // Is this file a symbolic link?
            S_ISBLK( buf.st_mode )  ||  // Is this file a block device?
            !S_ISREG( buf.st_mode ) )   // Is this file non-regular?
            return 0;
        else
            return 1;
    }
        return -1;
}
/*  Return value:
 *  1  - The specified path is a directory
 *  0  - The specified path is not a directory
 *  -1 - There was an error trying to access the file
 */
int isDirectory(const char* path)
{
    struct stat buf;
    if( stat(path, &buf) == 0 )
    {
        if( S_ISDIR( buf.st_mode ) )
            return 1;
        else
            return 0;
    } else
        return -1;
}

typedef struct Node element;
element *listsort(element *list, int is_circular, int is_double);

int main(int argc, char* argv[]) {
	int printSummary = 0, printShortSummary = 0, c;
    /**********************************************************************/
    /*  Do step 0: Parse arguments                                        */
    /**********************************************************************/
	while ((c = getopt (argc, argv, "hsS")) != -1)
		switch (c)
		{
			case 's':
				printSummary = 1;
				break;
			case 'S':
				printShortSummary = 1;
				break;
			case 'h':
				fprintf (stderr, "USAGE: %s [OPTION] [FILE]...\n", argv[0]);
				fprintf (stderr, "       -h Print help\n", argv[0]);
				fprintf (stderr, "       -s Print summary at end\n", argv[0]);
				fprintf (stderr, "       -S Print only the summary - don't show duplicate files\n", argv[0]);
			default:
				abort ();
		}
    /**********************************************************************/
    /*  Do step 1: Reduce all directories to file names. (see design.doc) */
    /**********************************************************************/
    
    /*  First we create nodes for each file/directory that was passed
     *  to this program as a argument
     */
    int i, wastedSpace = 0, numDuplicateFiles = 0;
    for(i = optind; i < argc; ++i)
    {
        struct Node* tmpNode;
        tmpNode = malloc(sizeof(struct Node));
        tmpNode->filename = strdup(argv[i]);
        addToTail(tmpNode);
    }
    /*  Next, we are going to search the list for directories.
     *  If a directory is found replace that directory with its contents.
     *  Also, if we find any symbolic links, devices, or other non-regular
     *  file types we will remove those nodes from our list.
     */
    
    struct Node* previous = NULL; 
    struct Node* iterator = head;

    /*  Loop through the linked list */
    while(iterator != NULL) 
    {
        /*  Test this node to see if it's a directory */
        if( isDirectory(iterator->filename) == 1 ) {
            /*  If so, we first add all the files inside of that directory */
            addDirectoryContent(iterator->filename, iterator);
            /*  Now, delete the directory node from our linked list */
            remNode(previous);
            /*  Iterator now points to a freed node, we must change iterator */
            if(previous == NULL)
                iterator = head;    // We've deleted from the head
            else
                iterator = previous->next;
        } else if ( isRegular(iterator->filename) != 1 ) {
            /*  This file is not regular, remove it from the list */
            remNode(previous);
            /*  Iterator now points to a freed node, we must change iterator */
            if(previous == NULL)
                iterator = head;    // We've deleted from the head
            else
                iterator = previous->next;
        } else {
            /*  This file is a regular file, first get it's size */
			iterator->size = getFileSize(iterator->filename);
			/* traverse to the next file in our list */
            previous = iterator;
            iterator = iterator->next;
        }
    }
	/******************/
    /*  Do step 1.b:  */
    /******************/
	// Sort the list
	head = listsort(head, 0, 0);



    /******************/
    /*  Do step 2: */
    /***************/
    int foundIt = 0;
    /*  Loop through the linked list, this time comparing the contents of each file */
    /*  When we are completely done looping, there should be nothing left in the list */
    while(head != NULL)
    {
        previous = NULL;
        iterator = head->next;
        while(iterator != NULL)
        {
            /*  First test the file sizes for equality */
			// This was waaaaaay too inefficient
            if(head->size == iterator->size)
            {
                FILE* pFile1 = fopen(     head->filename, "r");
                FILE* pFile2 = fopen( iterator->filename, "r");
                
                if(pFile1 == NULL) {
                    printf("There was an error opening \"%s\"\n", head->filename);
                    if(pFile2 != NULL)
                        fclose(pFile2);
					break;
                }
                else if(pFile2 == NULL) {
                    printf("There was an error opening \"%s\"\n", iterator->filename);
                    if(pFile1 != NULL)
                        fclose(pFile1);
					remNode(previous);
					if(previous == NULL)
						iterator = head;
					else
						iterator = previous->next;
                }
                else {
                    char byte1;
                    char byte2;
                    /*  Next, test byte for byte until a mismatch */
                    do
                    {
                        if(pFile1 == NULL || pFile2 == NULL)
                            printf("ONE OF THE FILE DESCRIPTORS WAS NULL!\n");
                        if( feof(pFile1) || feof(pFile2) )
                        {
                            /*  End of file found, print out that there was a match! */
							if(!printShortSummary) {
								if(!foundIt)
									//  First match found, print the head file
									printf( "%s ", head->filename );
								printf( "%s ", iterator->filename );
							}
							numDuplicateFiles++;
							wastedSpace += iterator->size;

                            /*  Remember that we found at least one match for this file */
                            foundIt = 1;
                            /*  Finally, we must delete this node from out list */
                            remNode(previous);
                            /*  Iterator now points to a freed node, we must change iterator */
                            if(previous == NULL)
                                iterator = head;    // We've deleted from the head
                            else
                                iterator = previous->next;
                            break;
                        }
                        fread(&byte1, 1, 1, pFile1);
                        fread(&byte2, 1, 1, pFile2);

                    }while(byte1 == byte2);
                    fclose(pFile1);
                    fclose(pFile2);
                }            
            } else {
				break;
			}
			// End if( ***test for file size equality*** )
            previous = iterator;
            iterator = iterator->next;
        }// End: while(iterator != NULL)

		if(!printShortSummary) {		
			if(foundIt)
				printf("\n");   // Since we found at least one match, print a newline
		}
        foundIt = 0;
        /*  Finally, we must delete from the head of our list */
        remNode(NULL);
    }
	/*
	 * Print the summary out to the user
	 */
	if(printSummary || printShortSummary) {
		printf("\n");
		printf("# duplicate files: %d\n", numDuplicateFiles);
		printf("# duplicate bytes: %d\n", wastedSpace);
	}
}
