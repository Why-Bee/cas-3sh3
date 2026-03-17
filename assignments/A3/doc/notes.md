# Notes for A3

There are 2^15 bytes in physical memory, which means 2^15/2^8 = 128 physical pages

Logically, we have 2^16 bytes of virtual memory, which means 2^16/2^8 = 256 pages

## Initial
- page table starts with all invalid entries
- TLB starts with all invalid entries
- physical memory access pointer at the start of physical memory

## Read memory address:
- convert to page number
- check TLB for page number
  - if TLB hit, get physical page number and offset, access physical memory
  - if TLB miss, check page table for page number
    - if page table hit, get physical page number and offset, access physical memory, update TLB
    - if page table miss, page fault occurs, load page from disk to physical memory, update page table and TLB, access physical memory

## Page faults:
- open backing store
- try to write to physical memory, if the memory is full then evict a page (circular array, so just evict the next page in the circular array), update page table and TLB, write the new page to physical memory, update page table and TLB for the new page

## Other notes:
- TLB size is 16 and is also a circular array
- 128 frames so half of the virtual pages can be loaded into physical memory at a time



