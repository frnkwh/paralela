# Makefile for bsearch programs

CC = gcc
CFLAGS = -Wall -pthread
SOURCES_COMMON = chrono.c

# Target for bsearch_multiple
TARGET_B = bsearch_multiple
SOURCES_B = bsearch_multiple.c $(SOURCES_COMMON)
OBJECTS_B = $(SOURCES_B:.c=.o)

# Target for bsearch_single
TARGET_A = bsearch_single
SOURCES_A = bsearch_single.c $(SOURCES_COMMON)
OBJECTS_A = $(SOURCES_A:.c=.o)

all: $(TARGET_B) $(TARGET_A)

# Compile bsearch_multiple when typing `make b`
b: $(TARGET_B)

$(TARGET_B): $(OBJECTS_B)
	$(CC) -o $@ $^

# Compile bsearch_single when typing `make a`
a: $(TARGET_A)

$(TARGET_A): $(OBJECTS_A)
	$(CC) -o $@ $^

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS_B) $(OBJECTS_A) $(TARGET_B) $(TARGET_A)

.PHONY: all clean b a

