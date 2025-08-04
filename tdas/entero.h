#ifndef ENTERO_H
#define ENTERO_H

#include <stdbool.h>
#include <stddef.h>

typedef struct entero entero_t;

// Constructores
entero_t *entero_cero(void);
entero_t *entero_uno(void);

// Getter
char *entero_a_bcd(const entero_t *entero, size_t *n);
bool entero_imprimir(const entero_t *entero);

// Setter
entero_t *entero_desde_bcd(const char bcd[], size_t n);

// Destructor
void entero_destruir(entero_t *entero);

// Primitivas
entero_t *entero_clonar(const entero_t *entero);
bool entero_desplazar_derecha(entero_t *e);
bool entero_desplazar_izquierda(entero_t *e);
int entero_comparar(const entero_t *a, const entero_t *b);
bool entero_sumar(entero_t *a, const entero_t *b);

bool entero_restar(entero_t *a, const entero_t *b);
bool entero_dividir(entero_t *dividendo, const entero_t *divisor, entero_t **resto);
bool entero_multiplicar(entero_t *a, const entero_t *b);


// no son propias del tda son solo para pruebas de entero_multiplicar
// bool entero_a_izquierda(entero_t *e);
// bool entero_a_derecha(entero_t *e);
// bool entero_partir(const entero_t *e, size_t m, entero_t **e1, entero_t **e0);
// size_t entero_digitos(entero_t *e);

#endif
