CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = fs_indexed
SRCS = main.c fs_indexed.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c fs_indexed.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
