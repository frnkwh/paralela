# Configurações
CC = gcc
CFLAGS = -Wall -pthread
SOURCES = chrono.c mult_part.c verifica.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = multpart

# Regra principal
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# Dependências dos objetos
chrono.o: chrono.c verifica.h
mult_part.o: mult_part.c verifica.h
verifica.o: verifica.c verifica.h

# Compilação dos objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)

# Marcando como 'phony'
.PHONY: all clean
