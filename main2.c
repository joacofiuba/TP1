#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "auxiliares.h"
#include "simbolos.h"
#include "calculo.h"


bool imprimir_decimales(const racional_t *r, size_t decimales){ // e.g. 617/228, d = 2
    entero_t *numerador = entero_clonar(racional_numerador(r)); // e.g. 617
    // entero_t *denominador = racional_denominador(r); // e.g. 228
    size_t digitos;
    

    if(decimales == 0){
        entero_dividir(numerador, racional_denominador(r), NULL);
        entero_imprimir(numerador); 
        entero_destruir(numerador);
        return true;
    }

    // x IR de racional_t numerador != denominador SIEMPRE
    bool numerador_es_mayor = entero_comparar(numerador, racional_denominador(r)) > 0 ? true : false; 
    
    char bcd_diez[] = {0, 1};
    entero_t *entero_diez = entero_desde_bcd(bcd_diez, 2);
    if(entero_diez == NULL){
        entero_destruir(numerador);
        return false;
    }
    
    for(size_t i = 0; i < decimales; i++) // obtengo 61700
        entero_multiplicar(numerador, entero_diez);
    entero_destruir(entero_diez); // ya no lo uso más
    
    entero_dividir(numerador, racional_denominador(r), NULL); // 61700 / 228 = 270 -> entero que contiene los digitos enteros y decimales del numero a imprimir.
    char *bcd = entero_a_bcd(numerador, &digitos); // [0, 7, 2] lo convierto a bcd
    entero_destruir(numerador); // ya no lo uso más
    if(bcd == NULL)
        return false;

    if(numerador_es_mayor){
       for(size_t i = digitos; i > decimales; i--) // imprime hasta donde empiezan los decimales
            printf("%d", bcd[i - 1]);
        printf(".");
        for(size_t i = decimales; i > 0; i--) // imprime los decimales dsps del punto
            printf("%d", bcd[i - 1]);
    }
    else{ // se imprime "0." seguido de tantos ceros como hagan falta hasta completar la "resolución" teniendo en cuenta que tienen que entrar todos los digitos del bcd 
        printf("0."); 
        for(size_t i = 0 ; i < (digitos - decimales); i++) // imprime
            printf("%d", 0);
        for(size_t i = digitos; i > 0; i--)
            printf("%d", bcd[i - 1]);
    }    
    free(bcd);
    return true;
}

bool es_numero(const char *s) {
    if(s == NULL || *s == '\0') return false;

    for(int i = 0; s[i] != '\0'; i++) {
        if(!isdigit(s[i])) return false;
    }

    return true;
}


int main(int argc, char *argv[]){
    int decimales = 8;
    bool en_modo_racional = false;

    if (argc > 2) {
        fprintf(stderr, "Solo se puede ingresar: ./calculadora [OPCIONES]\n"
                        "Para ver las opciones ingrese: ./calculadora ayuda\n");
        return 1;
    }

    if (argc == 2) {
        if (strcmp(argv[1], "ayuda") == 0) {
            printf("Calculadora TA130\n"
                   "Uso: ./calculadora [OPCIONES]\n"
                   "OPCIONES:\n"
                   "    ayuda: Muestra esta ayuda.\n"
                   "    racional: Selecciona salida racional.\n"
                   "    <n>: Salida con <n> dígitos de precisión.\n"
                   "OPERADORES:\n"
                   "    a+b: Suma\n"
                   "    a-b: Resta\n"
                   "    a*b: Multiplicación\n"
                   "    a/b: División\n"
                   "    a^b: Potencia entera\n"
                   "    _a: Inverso aditivo\n"
                   "    fact(a): Factorial\n"
                   "    abs(a): Valor absoluto\n"
                   "    inv(a): Inverso multiplicativo\n"
                   "    pi: Número π\n"
                   "    e: Número de Euler\n"
                   "    phi: Número de oro\n");
            return 0;
        } else if (strcmp(argv[1], "racional") == 0) {
            en_modo_racional = true;
        } else if (es_numero(argv[1])) {
            decimales = atoi(argv[1]);
        } else {
            fprintf(stderr, "Uso: ./calculadora [OPCIONES]\n"
                            "OPCIONES:\n"
                            "    ayuda: Muestra esta ayuda.\n"
                            "    racional: Selecciona salida racional.\n"
                            "    <n>: Salida con <n> dígitos de precisión.\n");
            return 1;
        }
    }

    while(1){
        
        char linea[MAX_LINEA];
        printf("INGRESE LA OPERACION>>>");
        if (fgets(linea, MAX_LINEA, stdin) == NULL) {
            break; // sale del while
        }
        cola_t *cola_infija = cola_de_entrada(linea);
    
        if(cola_infija == NULL) continue;; // podriamos agregar esto --> printf("Error al armar cola de entrada\n");

        cola_t *cola_postfija = cola_de_salida(cola_infija);
        if(cola_postfija == NULL){
            cola_destruir(cola_infija, free);
            continue;
        }
        cola_destruir(cola_infija, free);

        racional_t *r = resultado_computo_postfijo(cola_postfija);
        if(r == NULL){
            cola_destruir(cola_postfija, free);
            continue;
        }
        
        cola_destruir(cola_postfija, free);
        //
        
        //hasta aca tengo el resultado en modo racional
        if(en_modo_racional){
            if(!racional_imprimir(r)){
                racional_destruir(r);
                fprintf(stderr, "Error imprimiendo en modo racional\n");
                continue;
            }
        }
        else{
            if(!imprimir_decimales(r, decimales)){
                racional_destruir(r);
                fprintf(stderr, "Error imprimiendo en modo decimal\n");
                continue;
            }
        }
            
        racional_destruir(r);
        printf("\n");
        

    }

    return 0;
}
