/*
Assignment 3: Memory Management in OS

Author: Yash Bhatia - bhatiy1 - 400362372
Author: Khawja Labib - labibk - 400356836

Date: [TODAY]
*/

/* TODO:
Part 1: Port over lab3
Part 2: Implement TLB
Part 3: Handle Page Faults
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256
#define TLB_SIZE 16
#define logical_address_space 65536
#define physical_memory_size 32768
#define NUM_PAGES logical_address_space / PAGE_SIZE
#define OFFSET_BITS 8 // log2(PAGE_SIZE) = 8
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)

#define address_file "addresses.txt"
#define backing_store "BACKING_STORE.bin"

#define BUFFER_SIZE 10

// Simulated memory - circular array
static signed char physical_memory[physical_memory_size];

// Globals
static int page_table[NUM_PAGES];
static int tlb[TLB_SIZE][2]; // circular array of (page number, frame number) pairs

int init_page_table() {
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i] = -1; // -1 indicates page not in memory
    }
    return 0;
}

int get_frame_from_tlb(int page_number) {
    return -1;
}

int in_tlb(int page_number) {
    return -1; // TODO
}

int convert_to_frame(int page_number) {
    // check TLB first
    if (in_tlb(page_number)) {
        return get_frame_from_tlb(page_number);
    }
    // if not in TLB, check page table
    if (page_table[page_number] != -1) {
        handle_page_table_hit(page_number);
        return page_table[page_number];
    } else {
        handle_page_fault(page_number);
    }
}

int read_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, file)) {
        // Remove newline character if present
        char *newline = strchr(buf, '\n');
        if (newline) {
            *newline = '\0';
        }

        // Convert the string to an unsigned long long integer
        unsigned long long logical_address = strtoull(buf, NULL, 10);

        // Compute page number and offset
        unsigned int page_number = logical_address >> OFFSET_BITS;
        unsigned int offset = logical_address & OFFSET_MASK;

        // Use page number to look up TLB
        unsigned int frame_number = -1; // default to not found
        frame_number = convert_to_frame(page_number); // TODO: implement this function to check TLB and page table
        unsigned long long physical_address = ((unsigned long long)frame_number << OFFSET_BITS) | offset;

        // Read the value from physical memory
        int value = physical_memory[physical_address];

        // Output result
        printf("Virtual address: %llu Physical address: %llu Value: %d\n", logical_address, physical_address, value); // TODO: compute physical address and value from frame number and offset
    }

    fclose(file);
    return 0;
}

int main()
{
    init_page_table();
    return read_file(address_file);
}