#include <stdio.h>
#include "fs_indexed.h"

int main()
{
    // // YASH! THESE ARE JIST TEST CODE
    // int b1, b2;

    // printf("---- Initializing FS ----\n");
    // initFS();

    // printf("\n---- Initial Free Blocks ----\n");
    // printFreeBlocks();

    // printf("\n---- Allocating one block ----\n");
    // b1 = allocateFreeBlock();
    // printf("Allocated block: %d\n", b1);
    // printFreeBlocks();

    // printf("\n---- Allocating another block ----\n");
    // b2 = allocateFreeBlock();
    // printf("Allocated block: %d\n", b2);
    // printFreeBlocks();

    // printf("\nReturning block %d\n", b1);
    // testReturnBlockByID(b1);
    // printFreeBlocks();
    // return 0;

    // Function to test the file system implementation
    printf("---- BEGIN TEST --------\n");
    printf("---- Initializing FS ----\n");
    initFS();

    printf("\n---- Creating file 'alpha.txt' of size 3072 bytes ----\n");
    printf("Expecting 3 blocks allocated and FIBID=0\n");
    createFile("alpha.txt", 3072);
    listFiles();

    printf("\n---- Creating file 'beta.txt' of size 5120 bytes ----\n");
    printf("Expecting 5 blocks allocated and FIBID=1\n");
    createFile("beta.txt", 5120);
    listFiles();

    printf("\n---- Deleting file 'alpha.txt' ----\n");
    deleteFile("alpha.txt");
    listFiles();

    printf("\n---- Creating file 'gamma.txt' of size 1024 bytes ----\n");
    printf("Expecting 1 block allocated and FIBID=0 (reusing FIBID of deleted file)\n");
    createFile("gamma.txt", 1024);
    listFiles();

    printf("---- Test for allocating more blocks than available ----\n");
    printf("Attempting to create file 'delta.txt' of size 102400 bytes (100 blocks)\n");
    printf("Expecting failure due to insufficient free blocks\n");
    createFile("delta.txt", 102400);
    listFiles();

    printf("-----Allocate many small files to be more than max files limit -----\n");
    for (int i = 0; i < MAXIMUM_FILES + 2; i++)
    {
        char fileName[20];
        sprintf(fileName, "file%d.txt", i);
        printf("Creating file '%s' of size 1024 bytes\n", fileName);
        createFile(fileName, 1024); // each file needs 1 block
    }

    listFiles();

    printf("\n---- Final Free Blocks ----\n");
    printFreeBlocks();

    printf("---- END TEST --------\n");
    return 0;
}