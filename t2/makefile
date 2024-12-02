# Configurações
CC = gcc
CFLAGS = -Wall -pthread
SOURCES = chrono.c mult_part.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = multpart

# Regra principal
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# Compilação dos objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)

# Marcando como 'phony'
.PHONY: all clean
