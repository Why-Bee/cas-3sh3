
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread -std=c11
TARGET = A2
SRCS = A2.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
