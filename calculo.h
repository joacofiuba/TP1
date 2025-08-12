#ifndef CALCULO_H
#define CALCULO_H

#include "tdas/racional.h"
#include "tdas/cola.h"

racional_t *calculadora_absoluto(const racional_t *r); //TDA 2

racional_t *calculadora_factorial(const racional_t *a); //TDA 2

racional_t *calculadora_cambio_signo(const racional_t *a); //TDA 2

racional_t *calculadora_inverso(const racional_t *a); //TDA 2

racional_t *calculadora_potencia(const racional_t *a, const racional_t *b);

racional_t *resultado_computo_postfijo(cola_t *cola_postfijo);

#endif