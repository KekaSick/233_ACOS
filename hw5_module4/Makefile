CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
TARGET = array_sum_threads
SOURCE = array_sum_threads.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) 20

.PHONY: all clean run 