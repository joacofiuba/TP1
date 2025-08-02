#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <stdio.h>
#include "tdas/cola.h"
#include "tdas/pila.h"
// Clasificación
enum tipo {ENTERO, RACIONAL, FUNCION, OPERADOR, PARENTESIS_ABIERTO, PARENTESIS_CERRADO};
// Tabla de búsqueda
enum funcion {FACT, ABS, INV, PI, EULER, PHI, FUNC_INDEFINIDO};
enum operador{SUMA, RESTA, PRODUCTO, DIVISION, POTENCIA, CAMBIO_SIGNO, OP_INDEFINIDO};

struct simbolo; 


void limpiar_todo(cola_t *salida, pila_t *pila, struct simbolo *aux);

void destruir_simbolo(void *dato);

void destruir_arreglo_simbolos(struct simbolo **arreglo, size_t cantidad);

size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad);

void destruir_simbolo(void *dato);

cola_t *cola_de_entrada(const char *linea);

cola_t *cola_de_salida(cola_t *entrada);

struct simbolo *calculadora_suma(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_resta(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_producto(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_division(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_potencia(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_cambio_signo(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_factorial(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_absoluto(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_inverso(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_pi(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_phi(struct simbolo **arreglo_simbolos);

struct simbolo *calculadora_euler(struct simbolo **arreglo_simbolos);

struct simbolo *computo_postfijo(cola_t *cola_postfijo);

struct simbolo *operacion(struct simbolo *aux, struct simbolo **arreglo_simbolos);


#endif
