CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LFLAGS = -lm
# Regla principal
all: calculadora

calculadora: main2.o cola.o pila.o entero.o racional.o ej1.o calculo.o simbolos.o auxiliares.o
	gcc main2.o cola.o pila.o entero.o racional.o ej1.o calculo.o simbolos.o auxiliares.o -o calculadora $(LFLAGS)

main2.o: main2.c auxiliares.h simbolos.h calculo.h
	gcc main2.c -c $(CFLAGS)

auxiliares.o: auxiliares.c auxiliares.h tdas/cola.h tdas/pila.h tdas/racional.h tdas/entero.h simbolos.h
	gcc auxiliares.c -c $(CFLAGS)

simbolos.o: simbolos.c simbolos.h tdas/cola.h tdas/pila.h auxiliares.h
	gcc simbolos.c -c $(CFLAGS)

calculo.o: calculo.c calculo.h auxiliares.h tdas/cola.h tdas/pila.h tdas/racional.h tdas/entero.h simbolos.h tdas/ej1.h
	gcc calculo.c -c $(CFLAGS)

cola.o: tdas/cola.c tdas/cola.h
	gcc tdas/cola.c -c $(CFLAGS)

pila.o: tdas/pila.c tdas/pila.h
	gcc tdas/pila.c -c $(CFLAGS)

racional.o: tdas/racional.c tdas/racional.h tdas/entero.h 
	gcc tdas/racional.c -c $(CFLAGS) 

entero.o: tdas/entero.c tdas/entero.h 
	gcc tdas/entero.c -c $(CFLAGS)

ej1.o: tdas/ej1.c tdas/ej1.h 
	gcc tdas/ej1.c -c $(CFLAGS)
clean:
	rm *.o calculadora
