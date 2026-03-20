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
#include <sys/mman.h> // for mmap
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define PAGE_SIZE 256
#define TLB_SIZE 16
#define physical_memory_size 32768
#define NUM_FRAMES (physical_memory_size / PAGE_SIZE)
#define logical_address_space 65536
#define NUM_PAGES (logical_address_space / PAGE_SIZE)
#define OFFSET_BITS 8 // log2(PAGE_SIZE) = 8
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)

#define address_file "addresses.txt"
#define backing_store "BACKING_STORE.bin"

#define BUFFER_SIZE 10

// Globals
static int page_table[NUM_PAGES];
static int tlb[TLB_SIZE][2]; // circular array of (page number, frame number) pairs
static signed char *backing_store_ptr; // pointer to the backing store
static signed char physical_memory[physical_memory_size]; // Simulated circular array for physical memory
static int number_of_addresses = 0;
static int number_of_page_faults = 0;

// Function prototypes
int init_page_table();
int init_backing_store();
int destroy_backing_store();
int get_frame_from_tlb(int page_number);
int in_tlb(int page_number);
int handle_page_table_hit(int page_number);
int handle_page_fault(int page_number);
int find_and_invalidate_page(int frame_number);
int convert_to_frame(int page_number);
int read_file(const char* filename);


// Circular frame allocator state.
static int next_frame = 0;      // frame to fill/overwrite next
static int loaded_frames = 0;   // number of frames currently populated (max NUM_FRAMES)
int add_page_to_memory(int page_number) {
    // Select the destination frame. When memory is full, this frame will be replaced.
    int frame_to_use = next_frame;
    int memory_index = frame_to_use * PAGE_SIZE;

    if (loaded_frames == NUM_FRAMES) {
        //printf("DEBUG Page replacement needed for frame number %d\n", frame_to_use);

        int status = find_and_invalidate_page(frame_to_use);
        if (status == -1) {
            printf("DEBUG Error: could not find page to invalidate for frame number %d\n", frame_to_use);
            exit(1);
        }
    }

    //printf("DEBUG Adding page number %d to frame %d (index %d)\n", page_number, frame_to_use, memory_index);

    memcpy(physical_memory + memory_index, backing_store_ptr + (page_number * PAGE_SIZE), PAGE_SIZE); // copy the page from the backing store to physical memory

    if (loaded_frames < NUM_FRAMES) {
        loaded_frames++;
    }
    next_frame = (next_frame + 1) % NUM_FRAMES;

    return frame_to_use;
}

int init_page_table() {
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i] = -1; // -1 indicates page not in memory
    }
    return 0;
}

int init_backing_store() {
    // Map the backing store to memory using mmap
    int fd = open(backing_store, O_RDONLY);

    if (fd == -1) {
        printf("Error opening backing store file: %s\n", backing_store);
        return 1;
    }

    signed char *backing_store_ptr_local = mmap( \
        0,
        logical_address_space, // size of the mapping
        PROT_READ, // read-only
        MAP_PRIVATE, // private mapping
        fd, // file descriptor
        0 // offset
    );
    close(fd);

    if (backing_store_ptr_local == MAP_FAILED) {
        printf("Error mapping backing store file: %s\n", backing_store);
        return 1;
    }

    backing_store_ptr = backing_store_ptr_local;
    return 0;
}

int destroy_backing_store() {
    if (munmap(backing_store_ptr, logical_address_space) == -1) {
        printf("Error unmapping backing store file: %s\n", backing_store);
        return 1;
    }
    return 0;
}

int get_frame_from_tlb(int page_number) {
    return -1;
}

int in_tlb(int page_number) {
    return -1; // TODO
}

int handle_page_table_hit(int page_number) {
    int frame_number = page_table[page_number]; // A page table contains frame numbers indexed by page number
    return frame_number;
}

int handle_page_fault(int page_number) {
    // A page fault occurs when the requested page is not in memory. Need to load it from the backing store!
    number_of_page_faults++;
    // copy the page from the backing store to physical memory
    int frame_number = add_page_to_memory(page_number);

    // update the page table to indicate that the page is now in memory
    page_table[page_number] = frame_number;
    return frame_number;
}

int find_and_invalidate_page(int frame_number) {
    // search the page table for the page that contains the frame number we need to overwrite
    for (int i = 0; i < NUM_PAGES; i++) {
        if (page_table[i] == frame_number) {
            page_table[i] = -1; // invalidate the page table entry
            return i;
        }
    }

    // also need to invalidate the TLB entry if it exists, TODO

    return -1; // should never reach here if the frame number is valid
}

int convert_to_frame(int page_number) {
    int frame_number = -1; // default to not found

    // check TLB first
    if (in_tlb(page_number) == 1) {
        //printf("DEBUG TLB hit for page number %d\n", page_number);
        return get_frame_from_tlb(page_number);
    }
    // if not in TLB, check page table
    if (page_table[page_number] != -1) {
        //printf("DEBUG Page table hit for page number %d\n", page_number);
        frame_number = handle_page_table_hit(page_number);
    } else {
        //printf("DEBUG Page fault for page number %d\n", page_number);
        frame_number = handle_page_fault(page_number);
    }
    return frame_number;
}

int read_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 1;
    }

    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, file)) {
        // Remove newline character if present
        char *newline = strchr(buf, '\n');
        if (newline) {
            *newline = '\0';
        }

        number_of_addresses++;

        //printf("DEBUG Read logical address: %s\n", buf);

        // Convert the string to an unsigned long long integer
        unsigned long long logical_address = strtoull(buf, NULL, 10);

        // Compute page number and offset
        unsigned int page_number = logical_address >> OFFSET_BITS;
        unsigned int offset = logical_address & OFFSET_MASK;

        //printf("DEBUG Page number: %u Offset: %u\n", page_number, offset);

        // Use page number to look up TLB and/or page table
        unsigned int frame_number = -1; // default to not found
        frame_number = convert_to_frame(page_number); // TODO: implement this function to check TLB
        unsigned long long physical_address = ((unsigned long long)frame_number * PAGE_SIZE) + offset;

        // Read the value from physical memory
        signed char value = physical_memory[physical_address];

        // Output result
        printf("Virtual address: %llu Physical address = %llu Value=%d\n", logical_address, physical_address, value);
    }

    fclose(file);
    return 0;
}

int main()
{
    if (init_backing_store() != 0) {
        return 1;
    }
    //printf("Backing store initialized.\n");

    if (init_page_table() != 0) {
        destroy_backing_store();
        return 1;
    }
    //printf("Page table initialized.\n");

    if (read_file(address_file) != 0) {
        destroy_backing_store();
        return 1;
    }

    if (destroy_backing_store() != 0) {
        return 1;
    }

    // Print statistics
    printf("Total addresses = %d\n", number_of_addresses);
    printf("Page_faults = %d\n", number_of_page_faults);
    // TODO tlb hits

    return 0;
}