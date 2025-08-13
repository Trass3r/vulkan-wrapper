# Simple Makefile for wrapper tests
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
VULKAN_LIBS = -lvulkan

.PHONY: all clean test

all: test_nulldescriptor_logic test_bc_simple

test_nulldescriptor_logic: test_nulldescriptor_logic.c
	$(CC) $(CFLAGS) -o $@ $<

test_bc_simple: test_bc_simple.c
	$(CC) $(CFLAGS) -o $@ $<

test_bc_emulation: test_bc_emulation.c
	$(CC) $(CFLAGS) $(VULKAN_LIBS) -o $@ $<

test: test_nulldescriptor_logic test_bc_simple
	./test_nulldescriptor_logic
	./test_bc_simple

clean:
	rm -f test_nulldescriptor_logic test_bc_simple test_bc_emulation

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all        - Build basic tests"
	@echo "  test       - Build and run basic tests"
	@echo "  test_bc_emulation - Build full Vulkan BC test (requires Vulkan)"
	@echo "  clean      - Remove built tests"
	@echo "  help       - Show this help"