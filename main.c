#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>   
#define CHAR_MAX 128  


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

//funcion auxiliar para eliminar cada dato de la cola (cada dato es un struct simbolo)


struct simbolo {
    char s[CHAR_MAX];
    enum tipo t;
    size_t indice_tipo; //solo se usa con operadores y funciones
};

void destruir_simbolo(void *dato) {
    struct simbolo *simbolo = dato;
    free(simbolo);
}


cola_t *cola_de_entrada(const char *linea) {
    cola_t *entrada = cola_crear();
    size_t i = 0, balance_parentesis = 0;
    

    while(linea[i] != '\0') {
        if(isspace(linea[i])) {
            i++;        
            continue;
        }

        struct simbolo *aux = malloc(sizeof(struct simbolo));
        if(aux== NULL) return NULL;
        size_t j = 0; // Para llenar aux->s
        
        bool en_simbolo = false;

        // Número entero o racional
        if (isdigit(linea[i])) {
            aux->t = ENTERO;
            aux->s[j++] = linea[i++];
            while (isdigit(linea[i])) {
                aux->s[j++] = linea[i++];
            }

            // Detectar racional (con punto)
            if (linea[i] == '.' && isdigit(linea[i + 1])) {
                aux->s[j++] = linea[i++]; // incluir el punto
                while (isdigit(linea[i])) {
                    aux->s[j++] = linea[i++];
                }
                aux->t = RACIONAL;
            }

            aux->s[j] = '\0';
            cola_encolar(entrada, aux);
            continue;
        }

        // Función: secuencia de letras
        if (isalpha(linea[i])) {
            while (isalpha(linea[i])) {
                aux->s[j++] = linea[i++];
            }
            aux->s[j] = '\0';
            aux->t = FUNCION;
            size_t indice_funcion = cadena_a_enumerativo(aux->s, funciones, FUNC_INDEFINIDO);
            if (indice_funcion == FUNC_INDEFINIDO) {
                fprintf(stderr, "Error: función desconocida: %s\n", aux->s);
                free(aux);
                cola_destruir(entrada, destruir_simbolo); //crear una funcion para destruir cada elemento de la cola
                return NULL;
            }
            else{
                aux->indice_tipo = indice_funcion;
            }

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
                fprintf(stderr, "Se está cerrando un paréntesis que nunca se abrió!");
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
        

        //si no es ninguno de los anteriores, deberia ser un operador operador
        aux->t = OPERADOR;
        aux->s[j++] = linea[i++];
        aux->s[j] = '\0';
        size_t indice_operador = cadena_a_enumerativo(aux->s, operadores, OP_INDEFINIDO);
        if(indice_operador == OP_INDEFINIDO){
            free(aux);
            fprintf(stderr, "");//completar
            cola_destruir(entrada, destruir_simbolo ); //completar con la funcion
            return NULL;
        }
        aux->indice_tipo = indice_operador;
        cola_encolar(entrada, aux);

    } //aca termina el while principal

        //Si no se cierra el parentesis
        if(balance_parentesis != 0){
            fprintf(stderr, "Falta cerrar un paréntesis");
            cola_destruir(entrada, destruir_simbolo);
            return NULL;
        }

    return entrada;
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
        if(aux->t == ENTERO || aux->t == RACIONAL){
            if(!cola_encolar(salida, aux)){
                cola_destruir(salida, destruir_simbolo);
                pila_destruir(pila, destruir_simbolo);
                destruir_simbolo(aux);
                return NULL;
            }
        }
        else if(aux->t == FUNCION){
            if(!pila_apilar(pila, aux)){
                cola_destruir(salida, destruir_simbolo);
                pila_destruir(pila, destruir_simbolo);
                destruir_simbolo(aux);
                return NULL;
            }
        }
        else if(aux->t == OPERADOR){
            
            while(!pila_esta_vacia(pila)){
                struct simbolo *pila_tope = pila_ver_tope(pila);
                if(pila_tope->t == FUNCION || pila_tope->indice_tipo >= aux->indice_tipo){
                    if(!cola_encolar(salida, pila_desapilar(pila))){
                        cola_destruir(salida, destruir_simbolo);
                        pila_destruir(pila, destruir_simbolo);
                        destruir_simbolo(aux);
                        return NULL;
                    }
                }
                else{
                    break;
                }
            }

            if(!pila_apilar(pila, aux)){
                cola_destruir(salida, destruir_simbolo);
                pila_destruir(pila, destruir_simbolo);
                destruir_simbolo(aux);
                return NULL;
            }
        }

        else if(aux->t == PARENTESIS_ABIERTO){
            if(!pila_apilar(pila, aux)){
                cola_destruir(salida, destruir_simbolo);
                pila_destruir(pila, destruir_simbolo);
                destruir_simbolo(aux);
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
                    cola_destruir(salida, destruir_simbolo);
                    pila_destruir(pila, destruir_simbolo);
                    destruir_simbolo(aux);
                    return NULL;
                }
                
            }
            struct simbolo *pila_tope = pila_ver_tope(pila);
            if(pila_tope != NULL && pila_tope->t == FUNCION){
                if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
                    cola_destruir(salida, destruir_simbolo);
                    pila_destruir(pila, destruir_simbolo);
                    destruir_simbolo(aux);
                    return NULL;
                }
            }
        }   
    }
    while(!pila_esta_vacia(pila)){
        if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
            cola_destruir(salida, destruir_simbolo);
            pila_destruir(pila, destruir_simbolo);
            return NULL;
        }
    }
    pila_destruir(pila, destruir_simbolo);

    return salida;

}