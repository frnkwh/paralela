CC = gcc
CFLAGS = -Wall -pthread
SOURCES_COMMON = chrono.c

TARGET_B = multpart-1k
SOURCES_B = mult_part_1k.c $(SOURCES_COMMON)
OBJECTS_B = $(SOURCES_B:.c=.o)

TARGET_A = multpart-100k
SOURCES_A = mult_part_100k.c $(SOURCES_COMMON)
OBJECTS_A = $(SOURCES_A:.c=.o)

all: $(TARGET_B) $(TARGET_A)

b: $(TARGET_B)

$(TARGET_B): $(OBJECTS_B)
	$(CC) -o $@ $^

a: $(TARGET_A)

$(TARGET_A): $(OBJECTS_A)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS_B) $(OBJECTS_A) $(TARGET_B) $(TARGET_A)

.PHONY: all clean b a

