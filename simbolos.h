#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include <stdio.h>

#include "tdas/cola.h"



#define MAX_LINEA 256
#define CADENA_CORTA 8
#define CANT_OPERADORES 6
#define CANT_FUNCIONES 6
#define MAX_CANT_PARAMETROS 2
#define DIGITOS_CONSTANTES 6 // cant de digitos que contienen las constantes incluyendo el punto


// Clasificación
enum tipo {ENTERO, RACIONAL, FUNCION, OPERADOR, PARENTESIS_ABIERTO, PARENTESIS_CERRADO};
enum funcion {FACT, ABS, INV, PI, EULER, PHI};
enum operador{SUMA, RESTA, PRODUCTO, DIVISION, POTENCIA, CAMBIO_SIGNO};


// Declaraciones de arrays globales (definidos en .c)
extern const char *funciones[];
extern const size_t funcion_parametros[];
extern const char *operadores[];
extern const size_t operador_aridad[];
extern const char *constantes[];

struct simbolo {
    char s[MAX_LINEA];
    size_t n; //solo es util para los numeros (ENTERO O RACIONAL)
    enum tipo t;
    size_t indice; // solo es util para simbolos de tipo OPERADOR o FUNCION!

};

cola_t *cola_de_entrada(const char *linea);

cola_t *cola_de_salida(cola_t *entrada);






#endif