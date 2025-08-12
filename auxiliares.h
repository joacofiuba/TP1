#include <stdio.h>
#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "tdas/cola.h"
#include "tdas/pila.h"
#include "tdas/racional.h"
#include "tdas/entero.h"

#include "simbolos.h"



size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad);

void free_all(cola_t *salida, pila_t *pila, struct simbolo *aux);

racional_t *simbolo_a_racional(struct simbolo *numero);

void destruir_clones(entero_t *num_a, entero_t *den_a, entero_t* num_b, entero_t *den_b);

racional_t *operacion(struct simbolo *aux, racional_t **arreglo_racionales_a_computar);

void destruir_numeros_desapilados(racional_t **racionales, size_t cantidad);




#endif