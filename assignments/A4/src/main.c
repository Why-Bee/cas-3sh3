#include <stdio.h>
#include "fs_indexed.h"

int main()
{
    // YASH! THESE ARE JIST TEST CODE
    int b1, b2;

    printf("---- Initializing FS ----\n");
    initFS();

    printf("\n---- Initial Free Blocks ----\n");
    printFreeBlocks();

    printf("\n---- Allocating one block ----\n");
    b1 = allocateFreeBlock();
    printf("Allocated block: %d\n", b1);
    printFreeBlocks();

    printf("\n---- Allocating another block ----\n");
    b2 = allocateFreeBlock();
    printf("Allocated block: %d\n", b2);
    printFreeBlocks();

    printf("\nReturning block %d\n", b1);
    testReturnBlockByID(b1);
    printFreeBlocks();
    return 0;
}