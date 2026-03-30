// lab3b.c - Part II of the Lab 3 assignment
// Author: Yash Bhatia - bhatiy1 - 400362372
// Date: 2026-03-15

#include <sys/mman.h> // for mmap
#include <string.h> // for memcpy
#include <fcntl.h> // for open
#include <stdlib.h>
#include <stdio.h> // for printf

#define INT_SIZE 4 // size of int in bytes
#define NUM_INTS 10 // number of integers to read from the file
#define MEMORY_SIZE (NUM_INTS * INT_SIZE) // total memory size needed

int intArray[NUM_INTS]; // array to hold the integers read from the file
signed char *mmapfpptr; // pointer to the memory-mapped file

int read_file(const char *filename) {
    // Open the file for reading
    int mmap_fd = open(filename, O_RDONLY); // read only
    if (mmap_fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Memory-map the file
    mmapfpptr = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmap_fd, 0);

    // Retrieve the integers from the memory-mapped file
    for (int i = 0; i < NUM_INTS; i++) {
        // Copy the integer from the memory-mapped file to the intArray
        memcpy(intArray+i, mmapfpptr + (i * INT_SIZE), INT_SIZE);
    }

    // Unmap memory
    if (munmap(mmapfpptr, MEMORY_SIZE) == -1) {
        perror("Error unmapping memory");
    }
    
    // Now that the memory is unmapped, the integers should be in intArray
    // Test that by computing the sum. Expected - 92

    int sum = 0;
    for (int i = 0; i < NUM_INTS; i++) {
        sum += intArray[i];
    }

    printf("Sum of numbers = %d\n", sum);
    return 0;
}

int main() {
    const char *filename = "numbers.bin";
    return read_file(filename);
}