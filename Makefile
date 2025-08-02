CC = gcc
CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LDFLAGS = -lm

# Archivos objeto
OBJ = main.o cola.o calculadora.o pila.o

# Regla principal
all: calculadora

# Enlazado final
calculadora: $(OBJ)
	$(CC) $(OBJ) -o calculadora $(LDFLAGS)

# Compilación individual
main.o: main.c cola.h calculadora.h pila.h
	$(CC) $(CFLAGS) -c main.c

calculadora.o: calculadora.c calculadora.h cola.h pila.h
	$(CC) $(CFLAGS) -c calculadora.c

cola.o: cola.c cola.h
	$(CC) $(CFLAGS) -c cola.c

pila.o: pila.c pila.h
	$(CC) $(CFLAGS) -c pila.c

# Limpieza
clean:
	del /Q *.o calculadora.exe 2>nul
