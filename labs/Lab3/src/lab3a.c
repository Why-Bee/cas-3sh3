// lab3a.c - Part I of the Lab 3 assignment
// Author: Yash Bhatia - bhatiy1 - 400362372
// Date: 2026-03-15

// Stage 1: Just read the file contents

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10

int read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, file)) {
        printf("%s", buf);
    }

    fclose(file);
    return 0;
}

int main() {
    const char *filename = "labaddr.txt";
    return read_file(filename);
}