// Lab 4: File Block Allocation Simulator Header File
// Implement Indexed Allocation Scheme for a simulated file system
// bhatiy1 (400362372)
// Date: 30th March 2026

#ifndef FS_INDEXED_H
#define FS_INDEXED_H

#include <stdlib.h>
#include <string.h>

// Defines
#define BLOCK_SIZE 1024 // 1kb
#define TOTAL_BLOCKS 64
#define MAXIMUM_FILES 10 // up to 10 files can be created in the file system

// Structs to represent the file system components
typedef struct Block
{
    unsigned char data[BLOCK_SIZE];
    int blockNumber;
} Block;

typedef struct
{
    int blockPointers[TOTAL_BLOCKS]; // array of block pointers (block numbers)
    int count;                       // number of blocks allocated to the file
} IndexBlock;                        // Index Block struct

typedef struct
{
    int blockID;
    char fileName[20];
    int fileSize;           // in bytes
    int blockCount;         // number of blocks allocated to the file
    IndexBlock *indexBlock; // pointer to the index block of the file
} FIB;                      // File Information Block struct

typedef struct
{
    int blockID;
    char status; // '0' for free, '1' for allocated
} FIB_status;

typedef struct BlockNode
{
    int blockID;
    struct BlockNode *next;
} BlockNode; // Linked list node for free blocks

typedef struct
{
    BlockNode *freeBlockHead; // head of the free block linked list
    BlockNode *freeBlockTail; // tail of the free block linked list

    Block disk[TOTAL_BLOCKS];             // Simulated disk blocks
    FIB_status blockStatus[TOTAL_BLOCKS]; // Array to track block allocation status

    FIB fileList[MAXIMUM_FILES]; // Array to store file information blocks
    int numFiles;                // Number of files currently in the file system
} VCB;                           // Volume Control Block struct

typedef struct
{
    VCB l_vcb; // Volume Control Block
} FileSystem;

// Function prototypes
void initFS();
int createFile(const char *fileName, int fileSize);
int deleteFile(const char *fileName);
void listFiles();
// The following are utility functions for internal use
int allocateFreeBlock();
void returnFreeBlock(Block *block);
void printFreeBlocks();

//  YASH!  TEST FUNCCTION DELETE BEFORE SUBMISSION
void testReturnBlockByID(int blockID);

#endif
