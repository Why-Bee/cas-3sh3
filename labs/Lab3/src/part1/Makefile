# Makefile for Lab 3

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = lab3a
SRC = lab3a.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

