//FUNCIONES TOTALES (PARA ORGANIZAR EL CODIGO)
//este seria el publico
#include <stdio.h>
#include "cola.h"
#include "pila.h"
#include "racional.h"
#include "entero.h"


size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad); //ESTA ES UNA AUXILIAR

cola_t *cola_de_entrada(const char *linea); //TDA 1

void free_all(cola_t *salida, pila_t *pila, struct simbolo *aux); //AUXILIAR

cola_t *cola_de_salida(cola_t *entrada); //TDA 1

racional_t *simbolo_a_racional(struct simbolo *numero); //AUXILIAR


racional_t *calculadora_absoluto(const racional_t *r); //TDA 2

racional_t *calculadora_factorial(const racional_t *a); //TDA 2

racional_t *calculadora_cambio_signo(const racional_t *a); //TDA 2

racional_t *calculadora_inverso(const racional_t *a); //TDA 2

void destruir_clones(entero_t *num_a, entero_t *den_a, entero_t* num_b, entero_t *den_b); //auxiliar

racional_t *calculadora_potencia(const racional_t *a, const racional_t *b); //TDA 2

racional_t *operacion(struct simbolo *aux, racional_t **arreglo_racionales_a_computar); //AUXILIAR PARA REALIZAR EL CALCULO CORRESPONDIENTE

void destruir_numeros_desapilados(racional_t **racionales, size_t cantidad); //AUXILIAR

racional_t *resultado_computo_postfijo(cola_t *cola_postfijo); //TDA 1


//AUXILIARES PARA EL MAIN
bool imprimir_decimales(const racional_t *r, size_t decimales);

bool es_numero(const char *s);

//SUBDIVISION DE TDAS
//LAS FUNCIONES AUXILIARES LAS USO SOLAMENTE EN CADA .C QUE CORRESPONDA

// TDA 1 -->  se encarga de la entrada y salida de la calculadora

// TDA 2 --> se encarga de lo que pasa en el medio (calcular operaciones y funciones)



