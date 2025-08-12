#include <stdio.h>
#include <stdlib.h>
#include "auxiliares.h"
#include "calculo.h"

#include <string.h>

// busqueda generica
size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad){ //en cantidad mandarias CANT_OPERADORES o CANT_FUNCIONES
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(s, opciones[i])){
            return i;
        }
    }
    return cantidad + 1; // devuelve un valor fuera del rango del indice, indicando que no se encuentra en la tabla de busqueda
} //auxiliar 

//FUNCION AUXILIAR PARA LIBERAR MEMORIA EN CASO DE FALLA
void free_all(cola_t *salida, pila_t *pila, struct simbolo *aux) {
    if (salida != NULL) {
        cola_destruir(salida, free);
    }
    if (pila != NULL) {
        pila_destruir(pila, free);
    }
    if (aux != NULL) {
        free(aux);
    }
}

racional_t *simbolo_a_racional(struct simbolo *numero){
   
    if (!numero) return NULL;
    char bcd[numero->n];
    size_t i = numero->n, j = 0, cant_decimales = 0;
          
    if(numero->t == RACIONAL){ // leo la cadena al reves
        while(i > 0 && numero->s[i - 1] == '0') i--; //ignoro los ceros no significativos
    
        while (i > 0 && numero->s[i - 1] != '.') { //leo solo los decimales
            bcd[j++] = numero->s[(i--) - 1] - '0';
            cant_decimales++;
        }

        i--;

        while (i > 0) //leo la parte entera
            bcd[j++] = numero->s[(i--)-1] - '0'; //j tiene la cantidad de digitos del arreglo bcd
    }

    else // es entero
        while(i > 0)
            bcd[j++] = numero->s[(i--) - 1] - '0'; //j almacena la cantidad de digitos del arreglo
    

    entero_t *den = NULL;

    if(cant_decimales > 0){
        char bcd_diez[] = {0, 1};
        entero_t *entero_diez = entero_desde_bcd(bcd_diez, 2);
        
        if (!entero_diez) return NULL;

        den = entero_clonar(entero_diez);
        for (size_t k = 1; k < cant_decimales; k++) {
            if (!entero_multiplicar(den, entero_diez)) {
                entero_destruir(entero_diez);
                entero_destruir(den);
                return NULL;
            }
        }
        entero_destruir(entero_diez);
    }

    entero_t *numerador = entero_desde_bcd(bcd,j);
    racional_t *racional = racional_crear(false, numerador, den);
    entero_destruir(numerador);
    if(den != NULL) entero_destruir(den);

    return racional;
}

//auxiliar
void destruir_clones(entero_t *num_a, entero_t *den_a, entero_t* num_b, entero_t *den_b){
    entero_destruir(num_a);
    entero_destruir(den_a);
    entero_destruir(num_b);
    entero_destruir(den_b);
}

//FUNCION AUXILIAR QUE REALICE LAS OPERACIONES CORRESPONDIENTES DEL COMPUTO DEL POSTFIJO

racional_t *operacion(struct simbolo *aux, racional_t **arreglo_racionales_a_computar){//recibe el aux que se esta utilizando para el computo (funcion u operador) y el arreglo de estructuras con el numero o numeros 
    if(aux->t == OPERADOR && arreglo_racionales_a_computar!= NULL){
        if(aux->indice == SUMA){
            return racional_sumar(arreglo_racionales_a_computar[1], arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == RESTA){
            return racional_restar(arreglo_racionales_a_computar[1], arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == PRODUCTO){
            return racional_multiplicar(arreglo_racionales_a_computar[1], arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == DIVISION){
            return racional_dividir(arreglo_racionales_a_computar[1], arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == POTENCIA){
            return calculadora_potencia(arreglo_racionales_a_computar[1], arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == CAMBIO_SIGNO){
            return calculadora_cambio_signo(arreglo_racionales_a_computar[0]);
        }
    }
    else if(aux->t == FUNCION && arreglo_racionales_a_computar != NULL){
        if(aux->indice == FACT){
            return calculadora_factorial(arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == ABS){
            return calculadora_absoluto(arreglo_racionales_a_computar[0]);
        }
        else if(aux->indice == INV){
            return calculadora_inverso(arreglo_racionales_a_computar[0]);
        }
    }
    return NULL; //chequear despues los return
}


//FUNCION AUXILIAR PARA LIIBERAR ARREGLO DE PUNTEROS A STRUCT SIMBOLO


void destruir_numeros_desapilados(racional_t **racionales, size_t cantidad){
    for(size_t i = 0; i < cantidad; i++){
        racional_destruir(racionales[i]);
    }
    free(racionales);
}



