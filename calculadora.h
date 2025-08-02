#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <stdio.h>
#include "cola.h"
#include "pila.h"
// Clasificación
enum tipo {ENTERO, RACIONAL, FUNCION, OPERADOR, PARENTESIS_ABIERTO, PARENTESIS_CERRADO};

// Tabla de búsqueda
enum funcion {FACT, ABS, INV, PI, EULER, PHI, FUNC_INDEFINIDO};
const char *funciones[] = {
    [FACT] = "fact",
    [ABS] = "abs",
    [INV] = "inv",
    [PI] = "pi",
    [EULER] = "e",
    [PHI] = "phi"
};
enum operador{SUMA, RESTA, PRODUCTO, DIVISION, POTENCIA, CAMBIO_SIGNO, OP_INDEFINIDO};
const char *operadores[] = {
    [SUMA] = "+",
    [RESTA] = "-",
    [PRODUCTO] = "*",
    [DIVISION] = "/",
    [POTENCIA] = "^",
    [CAMBIO_SIGNO] = "_",
};

//tabla de busqueda general
size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad){ //en cantidad mandarias OP_INDEFINIDO o FUNC_INDEFINIDO
    for(size_t i=0; i < cantidad; i++){
        if(!strcmp(s, opciones[i])){
            return i;
        }
    }
    return cantidad; //DEVUELVE INDEFINIDO EN CASO DE QUE NO ESTE
}




struct simbolo {
    char s[CHAR_MAX];
    enum tipo t;
    size_t indice_tipo; //solo se usa con operadores y funciones (enumerativos). en los operadores sirve para la prioridad  
    size_t cant_parametros; //sirve solo para las funciones
    size_t aridad; //sirve solo para los operadores
};

void limpiar_todo(cola_t *salida, pila_t *pila, struct simbolo *aux);

void destruir_simbolo(void *dato);

enum tipo {ENTERO, RACIONAL, FUNCION, OPERADOR, PARENTESIS_ABIERTO, PARENTESIS_CERRADO};

enum funcion {FACT, ABS, INV, PI, EULER, PHI, FUNC_INDEFINIDO};

const char *funciones[];
enum operador{SUMA, RESTA, PRODUCTO, DIVISION, POTENCIA, CAMBIO_SIGNO, OP_INDEFINIDO};
const char *operadores[];

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