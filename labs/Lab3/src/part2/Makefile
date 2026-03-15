# Makefile for Lab 3b

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = lab3b
SRC = lab3b.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

