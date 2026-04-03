// fs_indexed.c
// File System Simulator driver source file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_indexed.h" // our custom header file

/* Data members */
static FileSystem fs;
/***********************************************************************************************/
/* File system functions */
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

int createFile(const char *fileName, int fileSize)
{
    // Create a new file with the given name and size, and return 0 on success, -1 on failure
    // To create a file, we need to follow the steps in the assignment spec

    // Check that the total number of files does not exceed MAXIMUM_FILES
    if (fs.l_vcb.numFiles >= MAXIMUM_FILES)
    {
        printf("Error: cannot create file '%s' - maximum number of files reached.\n", fileName);
        return ERROR_MAX_FILES_REACHED;
    }

    // Check that a file with the same name does not already exist
    for (int i = 0; i < fs.l_vcb.numFiles; i++)
    {
        if (strcmp(fs.l_vcb.fileList[i].fileName, fileName) == 0) // compare the filelist Filename with the new fileName
        {
            printf("Error: cannot create file '%s' - file already exists.\n", fileName);
            return ERROR_FILE_EXISTS;
        }
    }

    // Verify the number of blocks needed is <= available free blocks
    int blocksNeeded = (fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (blocksNeeded > numberOfFreeBlocks())
    {
        printf("Error: cannot create file '%s' - insufficient free space (need %d blocks, only %d available).\n",
               fileName, blocksNeeded, numberOfFreeBlocks());
        return ERROR_INSUFFICIENT_SPACE;
    }

    // Allocate data blocks using allocateFreeBlock() using indexed allocation
    // Then allocate an index block to store pointers to the data blocks
    IndexBlock *indexBlock = (IndexBlock *)malloc(sizeof(IndexBlock));
    if (indexBlock == NULL)
    {
        printf("Error: failed to create file '%s' - malloc failed for index block.\n", fileName);
        return -1;
    }
    for (int i = 0; i < blocksNeeded; i++)
    {
        int freeBlockID = allocateFreeBlock();
        if (freeBlockID == -1)
        {
            printf("Error: failed to create file '%s' - unexpected error allocating data block.\n", fileName);
            free(indexBlock);
            return -1;
        }
        indexBlock->blockPointers[i] = freeBlockID;
        indexBlock->count++;
    }

    // Allocate a file information block ID to the new file

    int fibID = getFileInformationBlockID();
    if (fibID == -1)
    {
        printf("Error: failed to create file '%s' - no available FIB entry.\n", fileName);
        free(indexBlock);
        return ERROR_MAX_FILES_REACHED;
    }

    // Update the file information block with the file's metadata and index block pointer
    fs.l_vcb.fileList[fibID].blockID = fibID;
    strncpy(fs.l_vcb.fileList[fibID].fileName, fileName, sizeof(fs.l_vcb.fileList[fibID].fileName) - 1);
    fs.l_vcb.fileList[fibID].fileSize = fileSize;
    fs.l_vcb.fileList[fibID].blockCount = blocksNeeded;
    fs.l_vcb.fileList[fibID].indexBlock = indexBlock;

    // Increment the total number of files in the file system
    fs.l_vcb.numFiles++;

    printf("File '%s' created successfully: size = %d bytes, blocks allocated = %d.\n",
           fileName, fileSize, blocksNeeded);
    return 0;
}

int deleteFile(const char *fileName)
{
    // Delete the file with the given name, and return 0 on success, -1 on failure
    // To delete a file, we need to follow the steps in the assignment spec

    // Search for the file by name in the file information blocks
    int fibIndex = -1;
    for (int i = 0; i < fs.l_vcb.numFiles; i++)
    {
        if (strcmp(fs.l_vcb.fileList[i].fileName, fileName) == 0)
        {
            fibIndex = i;
            break;
        }
    }

    if (fibIndex == -1)
    {
        printf("Error: cannot delete file '%s' - file not found.\n", fileName);
        return ERROR_FILE_NOT_FOUND;
    }

    // Retrieve the index block pointer from the file information block
    IndexBlock *indexBlock = fs.l_vcb.fileList[fibIndex].indexBlock;

    int returnedBlocks = indexBlock->count;

    // Return all allocated data blocks to the free block linked list using returnFreeBlock()
    for (int i = 0; i < returnedBlocks; i++)
    {
        int blockID = indexBlock->blockPointers[i];
        returnFreeBlock(&fs.l_vcb.disk[blockID]);
    }

    // Free the index block memory
    free(indexBlock);

    // Reset the file information block entry to indicate it's now available
    resetFIB(fibIndex);

    // Decrement the total number of files in the file system
    fs.l_vcb.numFiles--;

    printf("File '%s' deleted successfully: %d blocks returned to free list.\n",
            fileName, returnedBlocks);
    return 0;
}

void listFiles()
{
    // List all files in the file system along with their metadata (name, size, blocks allocated)
    // Print format:
    // alpha.txt |   3072 bytes |  3 data blocks | FIBID=0
    // beta.txt  |   5120 bytes |  5 data blocks | FIBID=1
    // ...
    int num_files = fs.l_vcb.numFiles;
    printf("Root Directory Listing (%d files):\n", num_files);

    for (int i = 0; i < MAXIMUM_FILES; i++)
    {
        if (fs.l_vcb.fileList[i].blockID != -1) // means this FIB entry is allocated to a file
        {
            printf("%-20s | %10d bytes | %10d data blocks | FIBID=%d\n",
                   fs.l_vcb.fileList[i].fileName,
                   fs.l_vcb.fileList[i].fileSize,
                   fs.l_vcb.fileList[i].blockCount,
                   fs.l_vcb.fileList[i].blockID);
        }
    }
}
/***********************************************************************************************/
/* Helper functions */

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

void resetFIB(int i) // only helper used in initFS()
{
    fs.l_vcb.fileList[i].blockID = -1;
    fs.l_vcb.fileList[i].fileName[0] = '\0';
    fs.l_vcb.fileList[i].fileSize = 0;
    fs.l_vcb.fileList[i].blockCount = 0;
    fs.l_vcb.fileList[i].indexBlock = NULL;
}

int numberOfFreeBlocks()
{
    BlockNode *current = fs.l_vcb.freeBlockHead;
    int count = 0;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}

int getFileInformationBlockID()
{
    for (int i = 0; i < MAXIMUM_FILES; i++)
    {
        if (fs.l_vcb.fileList[i].blockID == -1) // means this FIB entry is available
        {
            return i;
        }
    }
    return -1; // no available FIB ID
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