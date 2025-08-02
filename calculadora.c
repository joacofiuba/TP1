#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>   
#define CHAR_MAX 128  

#include "calculadora.h"
#include "cola.h"
#include "pila.h"
#include "ej1.h" //uso la funcion para pasar de binario a bcd


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
            
            if(indice_funcion >= FACT && indice_funcion <= INV) aux->cant_parametros = 1;
            else if(indice_funcion >= PI && indice_funcion <= PHI) aux->cant_parametros = 0;

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
        

        //si no es ninguno de los anteriores, deberia ser un operador 
        aux->t = OPERADOR;
        aux->s[j++] = linea[i++];
        aux->s[j] = '\0';
        size_t indice_operador = cadena_a_enumerativo(aux->s, operadores, OP_INDEFINIDO);
        if(indice_operador == OP_INDEFINIDO){
            free(aux);
            fprintf(stderr, "XDDD");//completar
            cola_destruir(entrada, destruir_simbolo ); 
            return NULL;
        }
        aux->indice_tipo = indice_operador;
        if(indice_operador >= SUMA && indice_operador <= POTENCIA) aux->aridad = 2;
        else if(indice_operador == CAMBIO_SIGNO) aux->aridad = 1;
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

//FUNCION AUXILIAR PARA LIBERAR MEMORIA EN CASO DE FALLA

void limpiar_todo(cola_t *salida, pila_t *pila, struct simbolo *aux) {
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
        if(aux->t == ENTERO || aux->t == RACIONAL){
            if(!cola_encolar(salida, aux)){
                limpiar_todo(salida, pila, aux);
                return NULL;
            }
        }
        else if(aux->t == FUNCION){
            if(!pila_apilar(pila, aux)){
                limpiar_todo(salida, pila, aux);
                return NULL;
            }
        }
        else if(aux->t == OPERADOR){
            
            while(!pila_esta_vacia(pila)){
                struct simbolo *pila_tope = pila_ver_tope(pila);
                if(pila_tope->t == FUNCION || (pila_tope->t == OPERADOR && pila_tope->indice_tipo >= aux->indice_tipo)){
                    if(!cola_encolar(salida, pila_desapilar(pila))){
                        limpiar_todo(salida, pila, aux);
                        return NULL;
                    }
                }
                else{
                    break;
                }
            }

            if(!pila_apilar(pila, aux)){
                limpiar_todo(salida, pila, aux);
                return NULL;
            }
        }

        else if(aux->t == PARENTESIS_ABIERTO){
            if(!pila_apilar(pila, aux)){
                limpiar_todo(salida, pila, aux);
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
                    limpiar_todo(salida, pila, aux);
                    return NULL;
                }
                
            }
            struct simbolo *pila_tope = pila_ver_tope(pila);
            if(pila_tope != NULL && pila_tope->t == FUNCION){
                if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
                    limpiar_todo(salida, pila, aux);
                    return NULL;
                }
            }
        }   
    }
    while(!pila_esta_vacia(pila)){
        if(!cola_encolar(salida, pila_desapilar(pila))){ //paso de la pila a la cola
            limpiar_todo(salida, pila, aux);
            return NULL;
        }
    }
    pila_destruir(pila, destruir_simbolo);

    return salida;

}

//FUNCION AUXILIAR QUE REALICE LAS OPERACIONES CORRESPONDIENTES DEL COMPUTO DEL POSTFIJO

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
}
//FUNCION AUXILIAR PARA LIIBERAR ARREGLO DE PUNTEROS A STRUCT SIMBOLO

void destruir_arreglo_simbolos(struct simbolo **arreglo, size_t cantidad) {

    for (size_t i = 0; i < cantidad; i++) {
        destruir_simbolo(arreglo[i]);  // libera el struct simbolo* i-ésimo
    }

    free(arreglo);  // libera el arreglo de punteros
}


//COMPUTO DEL POSTFIJO
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