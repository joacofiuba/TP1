CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LFLAGS = -lm
# Regla principal
all: calculadora

calculadora: main.o cola.o pila.o entero.o racional.o
	gcc main.o cola.o pila.o entero.o racional.o -o calculadora $(LFLAGS)

main.o: main.c tdas/cola.h tdas/pila.h tdas/entero.h tdas/racional.c
	gcc main.c -c $(CFLAGS)

cola.o: tdas/cola.c tdas/cola.h
	gcc tdas/cola.c -c $(CFLAGS)

pila.o: tdas/pila.c tdas/pila.h
	gcc tdas/pila.c -c $(CFLAGS)

racional.o: tdas/racional.c tdas/racional.h tdas/entero.h 
	gcc tdas/racional.c -c $(CFLAGS) 

entero.o: tdas/entero.c tdas/entero.h 
	gcc tdas/entero.c -c $(CFLAGS)

clean:
	rm *.o calculadora
