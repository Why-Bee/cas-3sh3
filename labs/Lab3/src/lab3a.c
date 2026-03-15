// lab3a.c - Part I of the Lab 3 assignment
// Author: Yash Bhatia - bhatiy1 - 400362372
// Date: 2026-03-15

// Stage 3: Add on the physical addresses translation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 10

// System parameters of my computer
#define PAGE_SIZE 4096 // got from getconf PAGE_SIZE
#define OFFSET_BITS 12 // log2(PAGE_SIZE) = 12
#define OFFSET_MASK (PAGE_SIZE - 1) // 0xFFF for 12 bits
#define PAGES 1024 // 2^(32 - OFFSET_BITS) = 2^20 = 1024

// from the given doc:
int page_table[PAGES] = {6, 4, 3, 7, 0, 1, 2, 5}; // example mapping

int read_file(const char *filename) {
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

        // Compute physical address: extract frame number of page and compute addr
        unsigned int frame_number = page_table[page_number];
        unsigned long long physical_address = ((unsigned long long)frame_number << OFFSET_BITS) | offset;

        // Print the results
        printf("Virtual addr is %llu: Page# = %u & Offset = %u. Physical addr = %llu.\n",
               logical_address, page_number, offset, physical_address);
    }

    fclose(file);
    return 0;
}

int main() {
    const char *filename = "labaddr.txt";
    return read_file(filename);
}