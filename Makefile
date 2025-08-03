CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LFLAGS = -lm
# Regla principal
all: calculadora

calculadora: main.o cola.o pila.o 
	gcc main.o cola.o pila.o -o calculadora $(LFLAGS)

main.o: main.c tdas/cola.h tdas/pila.h
	gcc main.c -c $(CFLAGS)

cola.o: tdas/cola.c tdas/cola.h
	gcc tdas/cola.c -c $(CFLAGS)

pila.o: tdas/pila.c tdas/pila.h
	gcc tdas/pila.c -c $(CFLAGS)

clean:
	rm *.o calculadora
