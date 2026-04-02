// fs_indexed.c
// File System Simulator driver source file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_indexed.h" // our custom header file

static FileSystem fs; // YASH! we dont want this to be global so changed this to static

// HELPERS
// helper to reset a FIB entry
static void resetFIB(int i) // only helper used in initFS()
{
    fs.l_vcb.fileList[i].blockID = -1;
    fs.l_vcb.fileList[i].fileName[0] = '\0';
    fs.l_vcb.fileList[i].fileSize = 0;
    fs.l_vcb.fileList[i].blockCount = 0;
    fs.l_vcb.fileList[i].indexBlock = NULL;
}

void initFS()
{
    int i;

    // initialize free list
    fs.l_vcb.freeBlockHead = NULL;
    fs.l_vcb.freeBlockTail = NULL;

    // initialize number of files
    fs.l_vcb.numFiles = 0;

    // initialize disk blocks and block status table
    for (i = 0; i < TOTAL_BLOCKS; i++)
    {
        fs.l_vcb.disk[i].blockNumber = i;

        fs.l_vcb.blockStatus[i].blockID = i;
        fs.l_vcb.blockStatus[i].status = '0'; // free
    }

    // initialize file list
    for (i = 0; i < MAXIMUM_FILES; i++)
    {
        resetFIB(i);
    }

    // build free block linked list: 0 -> 1 -> 2 -> ... -> TOTAL_BLOCKS-1
    for (i = 0; i < TOTAL_BLOCKS; i++)
    {
        BlockNode *newNode = (BlockNode *)malloc(sizeof(BlockNode));
        if (newNode == NULL)
        {
            printf("Error: file system initialization failed (malloc failed).\n");
            return;
        }

        newNode->blockID = i;
        newNode->next = NULL;

        if (fs.l_vcb.freeBlockHead == NULL)
        {
            fs.l_vcb.freeBlockHead = newNode;
            fs.l_vcb.freeBlockTail = newNode;
        }
        else
        {
            fs.l_vcb.freeBlockTail->next = newNode;
            fs.l_vcb.freeBlockTail = newNode;
        }
    }

    printf("File system initialized successfully: %d blocks created, block size = %d bytes.\n",
           TOTAL_BLOCKS, BLOCK_SIZE);
}

int allocateFreeBlock()
{
    BlockNode *temp;
    int freeBlockID;

    // no free blocks available
    if (fs.l_vcb.freeBlockHead == NULL)
    {
        return -1;
    }

    // remove from head
    temp = fs.l_vcb.freeBlockHead;
    freeBlockID = temp->blockID;
    fs.l_vcb.freeBlockHead = temp->next;

    // if list becomes empty, tail must also become NULL
    // edge case
    if (fs.l_vcb.freeBlockHead == NULL)
    {
        fs.l_vcb.freeBlockTail = NULL;
    }

    // mark this block as allocated
    fs.l_vcb.blockStatus[freeBlockID].status = '1';

    free(temp);
    return freeBlockID;
}

void returnFreeBlock(Block *block)
{
    BlockNode *newNode;
    int blockID;

    if (block == NULL)
    {
        return;
    }

    blockID = block->blockNumber;

    newNode = (BlockNode *)malloc(sizeof(BlockNode));
    if (newNode == NULL)
    {
        printf("Error: failed to return block %d to free list.\n", blockID);
        return;
    }

    newNode->blockID = blockID;
    newNode->next = NULL;

    // append to tail
    if (fs.l_vcb.freeBlockTail == NULL) // edge case: list is empty
    {
        fs.l_vcb.freeBlockHead = newNode;
        fs.l_vcb.freeBlockTail = newNode;
    }
    else
    {
        fs.l_vcb.freeBlockTail->next = newNode;
        fs.l_vcb.freeBlockTail = newNode;
    }

    // mark as free
    fs.l_vcb.blockStatus[blockID].status = '0';
}

void printFreeBlocks()
{
    BlockNode *current = fs.l_vcb.freeBlockHead;
    int count = 0;

    printf("Free Blocks: ");

    while (current != NULL)
    {
        printf("%d ", current->blockID);
        count++;
        current = current->next;
    }

    printf("\nTotal free blocks: %d\n", count);
}

//  YASH! TEST FUNCTION FOR RETURNFREEBLOCK DELETE BEFORE SUBMISSION
// !!!!!!!
// !!!!!!!
// !!!!!!!
void testReturnBlockByID(int blockID)
{
    if (blockID < 0 || blockID >= TOTAL_BLOCKS)
    {
        printf("Invalid block ID\n");
        return;
    }

    returnFreeBlock(&fs.l_vcb.disk[blockID]);
}