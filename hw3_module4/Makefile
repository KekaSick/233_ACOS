CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = regex_replace
SOURCE = regex_replace.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	@echo "Testing regex replacement..."
	@echo "Test 1: Replace 'world' with 'universe'"
	./$(TARGET) "world" "Hello world! Welcome to the world." "universe"
	@echo ""
	@echo "Test 2: Replace digits with 'X'"
	./$(TARGET) "[0-9]+" "The year is 2024 and the price is 99.99" "X"
	@echo ""
	@echo "Test 3: Replace multiple spaces with single space"
	./$(TARGET) "[ ]+" "This   has    multiple     spaces" " "

.PHONY: all clean test 