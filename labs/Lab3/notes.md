# Lab 3

## Part 1 - Overview
- Simulate a Memory Management Unit's address translation capabilities
- Use 20 logical addresses from `labaddr.txt` to test program

### Program Flow
1. Read address
2. Output virtual address
3. Output page number and offset
4. Write corresponding physical address

### Details
- Using `getconf` to get Page Size
- Examples suggest that the number of pages should be computed using 4 GiB address space divided by page size
- page table mapping also taken from the lab document

## Part 2 - Overview
- Learn about Memory Mapping files in C
- use system call `mmap()` to map `numbers.bin` into memory region
- this file has 10 4-byte ints in binary format

### Program Flow
1. Open `numbers.bin` using `open()`
2. Map this file to memory
3. Retrieve memory mapped contents using `memcpy()`
4. Unmap the file
5. Loop through the array to sum the integers and print result.

### Details
- Must not read the file directly using `read()`, only through memory mapping
- Use `munmap()` to unmap the file after use
- since doing low-level memory management, be careful of unsafe memory access and ensure proper cleanup of resources.