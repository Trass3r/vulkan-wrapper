# Simple Makefile for null descriptor tests
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean test

all: test_nulldescriptor_logic

test_nulldescriptor_logic: test_nulldescriptor_logic.c
	$(CC) $(CFLAGS) -o $@ $<

test: test_nulldescriptor_logic
	./test_nulldescriptor_logic

clean:
	rm -f test_nulldescriptor_logic

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all   - Build all tests"
	@echo "  test  - Build and run tests"
	@echo "  clean - Remove built tests"
	@echo "  help  - Show this help"