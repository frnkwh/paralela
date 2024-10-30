# Makefile for bsearch_multiple program

CC = gcc
CFLAGS = -Wall -pthread
TARGET = bsearch
SOURCES = bsearch_multiple.c chrono.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
