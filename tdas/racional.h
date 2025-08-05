#ifndef RACIONAL_H
#define RACIONAL_H


#include <stdbool.h>
#include "entero.h"

typedef struct racional racional_t;

// Crea un racional_t en base al signo, numerador y denominador dados. 
// El denominador podría valer NULL, en ese caso se considera que la 
// fracción se crea desde un numerador entero, es decir, el denominador
// se debe inicializar en uno.

// Constructor
racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador);	

// Destructor
void racional_destruir(racional_t *q);



// Utilidades
bool racional_imprimir(const racional_t *q);

// Getters
bool racional_es_negativo(const racional_t *q);
const entero_t *racional_numerador(const racional_t *q);
const entero_t *racional_denominador(const racional_t *q);

// Operaciones
racional_t *racional_sumar(const racional_t *q, const racional_t *r);
racional_t *racional_restar(const racional_t *q, const racional_t *r);
racional_t *racional_multiplicar(const racional_t *q, const racional_t *r);
racional_t *racional_dividir(const racional_t *q, const racional_t *r);

#endif
