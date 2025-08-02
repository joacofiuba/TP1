#include <stdio.h>
#include <stdlib.h>
#include "calculadora.h"
#include "cola.h"

#define MAX_LINEA 256

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

    struct simbolo *elemento_cola;
    while((elemento_cola = cola_desencolar(cola_infija)) != NULL){
        printf("%s\n", elemento_cola->s);
    }
    printf("termine je\n");
    return 0;
}