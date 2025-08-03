CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LFLAGS = -lm
# Regla principal
all: calculadora


calculadora: main.o cola.o pila.o 
	gcc main.o cola.o pila.o -o calculadora $(LFLAGS)

main.o: main.c tdas/cola.h tdas/pila.h calculadora.h
	gcc main.c -c $(CFLAGS)



cola.o: tdas/cola.c tdas/cola.h
	gcc cola.c -c $(CFLAGS)

pila.o: tdas/pila.c tdas/pila.h
	gcc pila.c -c $(CFLAGS)

# Limpieza
clean:
	rm *.o calculadora
