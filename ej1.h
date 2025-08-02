#ifndef EJ1_H
#define EJ1_H

#include <stdio.h>

void imprimir_bcd(const char bcd[], size_t digitos);

unsigned long bcd_a_binario(const char bcd[], size_t digitos);

size_t binario_a_bcd(unsigned long binario, char bcd[]);

size_t sumar_bcd(const char a[], size_t na, const char b[], size_t nb, char r[]);

size_t multiplicar_por_un_digito(const char a[], size_t na, char b, char r[]);

void imprimir_operacion(const char bcd1[], size_t n1, const char bcd2[], size_t n2, const char resultado[], size_t n_res, char operacion);





#endif