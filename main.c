#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>   

#include "tdas/cola.h"
#include "tdas/pila.h"
#include "tdas/racional.h"
#include "tdas/entero.h"

#include "ej1.h" //uso la funcion para pasar de binario a bcd


#define MAX_LINEA 256
#define CADENA_CORTA 8
#define CANT_OPERADORES 6
#define CANT_FUNCIONES 6


// Clasificación
enum tipo {ENTERO, RACIONAL, FUNCION, OPERADOR, PARENTESIS_ABIERTO, PARENTESIS_CERRADO};
enum funcion {FACT, ABS, INV, PI, EULER, PHI};
enum operador{SUMA, RESTA, PRODUCTO, DIVISION, POTENCIA, CAMBIO_SIGNO};

// tablas de busqueda
const char *funciones[] = {
    [FACT] = "fact",
    [ABS] = "abs",
    [INV] = "inv",
    [PI] = "pi",
    [EULER] = "e",
    [PHI] = "phi"
};

const size_t funcion_parametros[] = {
    [FACT] = 1,
    [ABS] = 1,
    [INV] = 1,
    [PI] = 0,
    [EULER] = 0,
    [PHI] = 0
}; 

const char *operadores[] = {
    [SUMA] = "+",
    [RESTA] = "-",
    [PRODUCTO] = "*",
    [DIVISION] = "/",
    [POTENCIA] = "^",
    [CAMBIO_SIGNO] = "_",
};

const size_t operador_aridad[] = {
    [SUMA] = 2,
    [RESTA] = 2,
    [PRODUCTO] = 2,
    [DIVISION] = 2,
    [POTENCIA] = 2,
    [CAMBIO_SIGNO] = 1
};

// Invariante: 0 <= t <= 5
// indice se inicializa <=>  tipo == OPERADOR || FUNCION
struct simbolo {
    char s[MAX_LINEA];
    enum tipo t;
    size_t indice; // solo es util para simbolos de tipo OPERADOR o FUNCION!
 };



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

// busqueda generica
size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad){ //en cantidad mandarias OP_INDEFINIDO o FUNC_INDEFINIDO
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(s, opciones[i])){
            return i;
        }
    }
    return cantidad + 1; // devuelve un valor fuera del rango del indice, indicando que no se encuentra en la tabla de busqueda
}


void destruir_simbolo(void *dato) {
    struct simbolo *simbolo = dato;
    free(simbolo);
}


cola_t *cola_de_entrada(const char *linea) {
    cola_t *entrada = cola_crear();
    size_t i = 0, j = 0, balance_parentesis = 0;
    

    while(linea[i] != '\0') {
        if(isspace(linea[i])) {
            i++;        
            continue;
        }

        struct simbolo *aux = malloc(sizeof(struct simbolo));
        if(aux== NULL) return NULL;
        j = 0; // Para llenar aux->s

        // Número entero o racional
        if (isdigit(linea[i])) {
            aux->t = ENTERO;
            aux->s[j++] = linea[i++];
            while(isdigit(linea[i])) {
                aux->s[j++] = linea[i++];
            }

            // Detectar racional (con punto)
            if (linea[i] == '.' && isdigit(linea[i + 1])) {
                aux->s[j++] = linea[i++]; // incluir el punto
                aux->t = RACIONAL;
                while(isdigit(linea[i]))
                    aux->s[j++] = linea[i++];
            }

            aux->s[j] = '\0';
            cola_encolar(entrada, aux);
            continue;
        }

        // Función: secuencia de letras
        if (isalpha(linea[i])) {
            aux->t = FUNCION;
            while(isalpha(linea[i]))
                aux->s[j++] = linea[i++];
            aux->s[j] = '\0';
                        
            size_t indice_funcion = cadena_a_enumerativo(aux->s, funciones, CANT_FUNCIONES);
            if (indice_funcion > CANT_FUNCIONES) { // => no existe la función
                fprintf(stderr, "Error: función desconocida: %s\n", aux->s);
                free(aux);
                cola_destruir(entrada, free); //crear una funcion para destruir cada elemento de la cola
                return NULL;
            }

            aux->indice = indice_funcion;
            cola_encolar(entrada, aux);
            continue;
        }


        // Paréntesis
        if(linea[i] == '('){
            balance_parentesis++;
            aux->t = PARENTESIS_ABIERTO;
            aux->s[j++] = linea[i++];
            aux->s[j] = '\0';
            cola_encolar(entrada, aux);
            continue;
        }
        if(linea[i] == ')'){
            balance_parentesis--;
            if(balance_parentesis < 0){
                fprintf(stderr, "¡Se está cerrando un paréntesis que nunca se abrió!");
                free(aux);
                cola_destruir(entrada, destruir_simbolo); //TERMINAR
                return NULL;
            }
            aux->t = PARENTESIS_CERRADO;
            aux->s[j++] = linea[i++];
            aux->s[j] = '\0';
            cola_encolar(entrada, aux);
            continue;
        }

        //si no es ninguno de los anteriores, deberia ser un operador 
        aux->t = OPERADOR;
        aux->s[j++] = linea[i++];
        aux->s[j] = '\0';

        size_t indice_operador = cadena_a_enumerativo(aux->s, operadores, CANT_OPERADORES);
        if(indice_operador > CANT_OPERADORES){
            free(aux);  
            fprintf(stderr, "Ingrese una linea con caracteres válidos");
            cola_destruir(entrada, destruir_simbolo ); 
            return NULL;
        }

        // if(indice_operador >= SUMA && indice_operador <= POTENCIA) aux->aridad = 2; // NO DEBERAI SER NECESARIO JEJEJE
        // else if(indice_operador == CAMBIO_SIGNO) aux->aridad = 1;
        
        aux->indice = indice_operador; 
        cola_encolar(entrada, aux);

    } //aca termina el while principal

     //Si no se cierra el parentesis
    if(balance_parentesis != 0){
        fprintf(stderr, "Falta cerrar un paréntesis \n");
        cola_destruir(entrada, destruir_simbolo);
        return NULL;
    }

    return entrada;
}

//FUNCION AUXILIAR PARA LIBERAR MEMORIA EN CASO DE FALLA
void free_all(cola_t *salida, pila_t *pila, struct simbolo *aux) {
    if (salida != NULL) {
        cola_destruir(salida, destruir_simbolo);
    }
    if (pila != NULL) {
        pila_destruir(pila, destruir_simbolo);
    }
    if (aux != NULL) {
        destruir_simbolo(aux);
    }
}


cola_t *cola_de_salida(cola_t *entrada){ //se devuelve la cola con notacion postfija (shunting yard)
    cola_t *salida = cola_crear();
    if(salida == NULL) return NULL;

    pila_t *pila = pila_crear();
    if(pila == NULL){
        cola_destruir(salida, destruir_simbolo);
        return NULL;
    }

    struct simbolo *aux;
    while((aux = cola_desencolar(entrada)) != NULL){
        if(aux->t == ENTERO || aux->t == RACIONAL){ // si es nro va cola de salida
            if(!cola_encolar(salida, aux)){
                free_all(salida, pila, aux);
                return NULL;
            }
        }

        else if(aux->t == FUNCION){ // si es función apila
            if(!pila_apilar(pila, aux)){
                free_all(salida, pila, aux);
                return NULL;
            }
        }

        else if(aux->t == OPERADOR){
            while(!pila_esta_vacia(pila)){
                struct simbolo *pila_tope = pila_ver_tope(pila);
                if(pila_tope->t == FUNCION || (pila_tope->t == OPERADOR && pila_tope->indice >= aux->indice)){
                    if(!cola_encolar(salida, pila_desapilar(pila))){
                        free_all(salida, pila, aux);
                        return NULL;
                    }
                }
                else break;
            }

            if(!pila_apilar(pila, aux)){
                free_all(salida, pila, aux);
                return NULL;
            }
        }

        else if(aux->t == PARENTESIS_ABIERTO){
            if(!pila_apilar(pila, aux)){
                free_all(salida, pila, aux);
                return NULL;
            }
        }
        else if(aux->t == PARENTESIS_CERRADO){
            while(!pila_esta_vacia(pila)){
                struct simbolo *pila_tope = pila_ver_tope(pila);
                if(pila_tope->t == PARENTESIS_ABIERTO){
                    destruir_simbolo(pila_desapilar(pila)); //descarto el (
                    break;                 
                }
                
                if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
                    free_all(salida, pila, aux);
                    return NULL;
                }
                
            }
            struct simbolo *pila_tope = pila_ver_tope(pila);
            if(pila_tope != NULL && pila_tope->t == FUNCION){
                if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
                    free_all(salida, pila, aux);
                    return NULL;
                }
            }
        }   
    }
    while(!pila_esta_vacia(pila)){
        if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
            free_all(salida, pila, aux);
            return NULL;
        }
    }
    pila_destruir(pila, destruir_simbolo);

    return salida;

}

//FUNCION AUXILIAR QUE REALICE LAS OPERACIONES CORRESPONDIENTES DEL COMPUTO DEL POSTFIJO
/*
struct simbolo *operacion(struct simbolo *aux, struct simbolo **arreglo_simbolos){//recibe el aux que se esta utilizando para el computo (funcion u operador) y el arreglo de estructuras con el numero o numeros 
    if(aux->t == OPERADOR && arreglo_simbolos != NULL){
        if(aux->indice_tipo == SUMA){
            return calculadora_suma(arreglo_simbolos);
        }
        else if(aux->indice_tipo == RESTA){
            return calculadora_resta(arreglo_simbolos);
        }
        else if(aux->indice_tipo == PRODUCTO){
            return calculadora_producto(arreglo_simbolos);
        }
        else if(aux->indice_tipo == DIVISION){
            return calculadora_division(arreglo_simbolos);
        }
        else if(aux->indice_tipo == POTENCIA){
            return calculadora_potencia(arreglo_simbolos);
        }
        else if(aux->indice_tipo == CAMBIO_SIGNO){
            return calculadora_cambio_signo(arreglo_simbolos);
        }
    }
    else if(aux->t == FUNCION && arreglo_simbolos != NULL){
        if(aux->indice_tipo == FACT){
            return calculadora_factorial(arreglo_simbolos);
        }
        else if(aux->indice_tipo == ABS){
            return calculadora_absoluto(arreglo_simbolos);
        }
        else if(aux->indice_tipo == INV){
            return calculadora_inverso(arreglo_simbolos);
        }
    }
    else if(aux->t == FUNCION && arreglo_simbolos == NULL){
        if(aux->indice_tipo == PI){
            
        }
        else if(aux->indice_tipo == EULER){

        }
        else if(aux->indice_tipo == PHI){
            
        }
    }
    return NULL; //chequear despues los return
}*/


//FUNCION AUXILIAR PARA LIIBERAR ARREGLO DE PUNTEROS A STRUCT SIMBOLO

void destruir_arreglo_simbolos(struct simbolo **arreglo, size_t cantidad) {

    for (size_t i = 0; i < cantidad; i++) {
        destruir_simbolo(arreglo[i]);  // libera el struct simbolo* i-ésimo
    }

    free(arreglo);  // libera el arreglo de punteros
}


//COMPUTO DEL POSTFIJO
/*
struct simbolo *computo_postfijo(cola_t *cola_postfijo){
    pila_t *pila = pila_crear();
    if(pila == NULL) return NULL;
    struct simbolo *aux;
    while((aux = cola_desencolar(cola_postfijo)) != NULL){
        if(aux->t == ENTERO || aux->t == RACIONAL){
            if(!pila_apilar(pila, aux)){
                limpiar_todo(NULL, pila, aux);
                return NULL;
            }
        }
        else if(aux->t == OPERADOR){
            struct simbolo **arreglo_simbolos = malloc(sizeof(struct simbolo *) * aux->aridad);
            if(arreglo_simbolos == NULL){
                limpiar_todo(NULL, pila, aux);
                return NULL;
            }
            size_t i=0;
            for(i=0; i < aux->aridad; i++){ //desapilo tantos numeros como correspondan segun la aridad
                arreglo_simbolos[i] = pila_desapilar(pila);//este arreglo de punteros a struct simbolo lo voy a mandar a la funcion operacion
                if(arreglo_simbolos[i] == NULL){ //quiere decir que la pila quedo vacia
                    fprintf(stderr, "CANTIDAD INVALIDA DE PARAMETROS\n"); //caso en el que haya menos parametros de los que se requiere
                    destruir_arreglo_simbolos(arreglo_simbolos, aux->aridad);
                    limpiar_todo(NULL, pila, aux);
                    return NULL;
                }
            }
           
            struct simbolo *resultado = operacion(aux, arreglo_simbolos); //este resultado lo voy a mandar a la pila
            if(!pila_apilar(pila, resultado)){
                destruir_simbolo(resultado);
                limpiar_todo(NULL, pila, aux);
                return NULL;
            }
            destruir_simbolo(aux);
            destruir_arreglo_simbolos(arreglo_simbolos, aux->aridad);

        }
        else if(aux->t == FUNCION){ //la cant de parametros puede ser 0 o 1
            if(aux->cant_parametros != 0){ //puede ser fact, abs o inv
                struct simbolo **arreglo_simbolos = malloc(sizeof(struct simbolo *) * aux->cant_parametros);
                if(arreglo_simbolos == NULL){
                    limpiar_todo(NULL, pila, aux);
                    return NULL;
                }
                for(size_t i=0; i < aux->cant_parametros; i++){ //desapilo tantos numeros como correspondan segun la aridad
                    arreglo_simbolos[i] = pila_desapilar(pila);//este arreglo de punteros a struct simbolo lo voy a mandar a la funcion operacion
                    if(arreglo_simbolos[i] == NULL){ //quiere decir que la pila quedo vacia
                        fprintf(stderr, "CANTIDAD INVALIDA DE PARAMETROS\n"); //caso en el que haya menos parametros de los que se requiere
                        destruir_arreglo_simbolos(arreglo_simbolos, aux->cant_parametros);
                        limpiar_todo(NULL, pila, aux);
                        return NULL;
                    }
                }
                struct simbolo *resultado = operacion(aux, arreglo_simbolos);
                if(!pila_apilar(pila, resultado)){
                    destruir_simbolo(resultado);
                    limpiar_todo(NULL, pila, aux);
                    return NULL;
                }
                destruir_simbolo(aux);
                destruir_arreglo_simbolos(arreglo_simbolos, aux->cant_parametros);
                
            }
            else if(aux->cant_parametros == 0){

            }            
        }
    } //una vez que termino de desencolar la cola de postfijo tengo el resultado en la pila
    //tengo que chequear que la pila tenga exactamente un elemento
    struct simbolo *resultado_final = pila_desapilar(pila);

    // Si después de desapilar hay *algo más*, es un error
    if (!pila_esta_vacia(pila)) {
        fprintf(stderr, "ERROR: La pila contiene más de un resultado al finalizar el cómputo (expresión mal formada)\n");
        limpiar_todo(NULL, pila, resultado_final);  // Limpio lo que queda
        return NULL;
    }

    pila_destruir(pila, destruir_simbolo); // Ya está vacía, no pasa nada
    return resultado_final;

}
*/
int main(){
    char linea[MAX_LINEA];
    printf("INGRESE UNA ENTRADA EN NOTACION INFIJA: \n");
    fgets(linea, MAX_LINEA, stdin);

    cola_t *cola_infija = cola_de_entrada(linea);
    
    if(cola_infija == NULL){
        printf("error al armar cola de entrada\n");
        return 1;
    }
   
    //voy a desencolar la cola en infija para chequear los resultados
    printf("COLA DE ENTRADA:\n");
    /*
    struct simbolo *elemento_cola_entrada;
    while((elemento_cola_entrada = cola_desencolar(cola_infija)) != NULL){
        printf("%s\n", elemento_cola_entrada->s);
    }
    */

    cola_t *cola_postfija = cola_de_salida(cola_infija);
    if(cola_postfija == NULL){
        printf("error al armar cola de salida\n");
        return 1;
    }
    printf("COLA DE SALIDA:\n");
    

    struct simbolo *elemento_cola_salida;
    while((elemento_cola_salida = cola_desencolar(cola_postfija)) != NULL){
        printf("%s\n", elemento_cola_salida->s);
        destruir_simbolo(elemento_cola_salida);
    }

    cola_destruir(cola_infija, free);

    cola_destruir(cola_postfija, free);

    printf("\n");
    return 0;
}
