# Notes on A4

## Where is the FS?
- `main` will not be having a FS object/instance
- It can only *call* methods to do the things mentioned
- The FileSystem will be a global statically-scoped struct within fs_indexed.c
- any accesses to FS (l_vcb) need to be done in fs_indexed.c

## TODO
- Maham: work on InitFS and any of the helper functions needed for InitFS by tomorrow morning
    - Write more helper functions if needed, such as printing blocks or w/e
- Yash: Pull repo and pick up from there