#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calculo.h"
#include "auxiliares.h"
#include "simbolos.h"


#include "tdas/cola.h"
#include "tdas/pila.h"
#include "tdas/entero.h"

#include "tdas/ej1.h"




//ABSOLUTO
racional_t *calculadora_absoluto(const racional_t *r){
    entero_t *numerador = entero_clonar(racional_numerador(r));
    entero_t *denominador = entero_clonar(racional_denominador(r));
    racional_t *absoluto = racional_crear(false, numerador, denominador);
    entero_destruir(numerador);
    entero_destruir(denominador);
    return absoluto;
}
//FACTORIAL

racional_t *calculadora_factorial(const racional_t *a){
    entero_t *uno = entero_uno();
    entero_t *cero = entero_cero();

    // Verificaciones
    if(entero_comparar(uno, racional_denominador(a)) != 0 || racional_es_negativo(a)){
        fprintf(stderr, "PARA FACTORIAL SOLO SE ACEPTAN NUMEROS NATURALES\n");
        entero_destruir(uno);
        entero_destruir(cero);
        return NULL;
    }

    // Caso base: 0! = 1
    if(entero_comparar(cero, racional_numerador(a)) == 0){
        racional_t *aux = racional_crear(false, uno, uno); //aca hay un cambio
        entero_destruir(uno);
        entero_destruir(cero);
        return aux;
    }

    // Caso general
    entero_t *i = entero_uno();
    entero_t *a_clon = entero_clonar(racional_numerador(a));
    entero_t *resultado = entero_uno(); // acumulador

    
    
    while(entero_comparar(i, a_clon) <= 0){
        entero_multiplicar(resultado, i);
        entero_sumar(i, uno);
    }
    

    racional_t *factorial = racional_crear(false, resultado, uno);
    entero_destruir(i);
    entero_destruir(a_clon);
    entero_destruir(resultado);
    entero_destruir(uno);
    entero_destruir(cero);

    return factorial;
}




//CAMBIO DE SIGNO

racional_t *calculadora_cambio_signo(const racional_t *a){ // hay que liberar la memoria luego de usar esta funcion en otro lugar
    entero_t *numerador = entero_clonar(racional_numerador(a));
    entero_t *denominador = entero_clonar(racional_denominador(a));
    racional_t *aux = racional_crear(!racional_es_negativo(a), numerador, denominador);
    entero_destruir(numerador);
    entero_destruir(denominador);
    return aux;
}
//INVERSO MULTIPLICATIVO

racional_t *calculadora_inverso(const racional_t *a){
    
    entero_t *numerador = entero_clonar(racional_numerador(a));
    entero_t *denominador = entero_clonar(racional_denominador(a));
    racional_t *inverso = racional_crear(racional_es_negativo(a), denominador, numerador);
    entero_destruir(numerador);
    entero_destruir(denominador);
    return inverso;
}

//POTENCIA
racional_t *calculadora_potencia(const racional_t *a, const racional_t *b){ //computa a^b
    entero_t *uno = entero_uno();
    entero_t *cero = entero_cero(); //hay que liberarlos
    
    entero_t *numerador_b_clon = entero_clonar(racional_numerador(b));
    entero_t *denominador_b_clon = entero_clonar(racional_denominador(b));

    entero_t *numerador_a_clon = entero_clonar(racional_numerador(a));
    entero_t *denominador_a_clon = entero_clonar(racional_denominador(a)); //hay que liberarlos


    //el racional b tiene que ser entero si o si
    if(entero_comparar(uno, racional_denominador(b)) != 0){
        fprintf(stderr, "PARA LA POTENCIA A^B, B TIENE QUE SER UN NUMERO ENTERO'\n");
        
        entero_destruir(uno);
        entero_destruir(cero);
        return NULL;
    }

    //CASOS EN LOS QUE B SEA UNO O CERO
    //CASO EN EL QUE B SEA 0 --> potencia = 1
    else if(entero_comparar(cero, racional_numerador(b)) == 0 && entero_comparar(cero, racional_numerador(a))!= 0){
        racional_t *potencia = racional_crear(false, uno, uno);
        entero_destruir(uno);
        entero_destruir(cero);
        destruir_clones(numerador_a_clon, denominador_a_clon, numerador_b_clon, denominador_b_clon);
        return potencia;
    }
    
    //CASO EN EL QUE B SEA 1 --> DEVUELVO a

    else if(entero_comparar(uno, racional_numerador(b)) == 0 && entero_comparar(uno,racional_denominador(b)) == 0){
        racional_t *potencia = racional_crear(racional_es_negativo(a), numerador_a_clon, denominador_b_clon);
        destruir_clones(numerador_a_clon, denominador_a_clon, numerador_b_clon, denominador_b_clon);
        entero_destruir(uno);
        entero_destruir(cero);  
        if(racional_es_negativo(b)){
            return calculadora_inverso(potencia);
        }

        return potencia;
    }

        
    entero_destruir(uno);
    entero_destruir(cero);

    //TODO ESTO COMPUTA A^B (SIN TENER EN CUENTA SI B ES POSITIVO O NEGATIVO)
    size_t n;
    char *bcd_numerador_b = entero_a_bcd(racional_numerador(b), &n);
    
    if(n >= 10){
        fprintf(stderr, "LA POTENCIA SE VA DE RANGO, MAXIMO UN NUMERO DE 9 DIGITOS\n");
        return NULL;
    }
    size_t numerador_b_en_binario = bcd_a_binario(bcd_numerador_b, n);
    free(bcd_numerador_b);
    
    
    racional_t *a_clon = racional_crear(racional_es_negativo(a), numerador_a_clon, denominador_a_clon);
    racional_t *potencia = a_clon;
    
    for(size_t i = 1; i < numerador_b_en_binario; i++){//MULTIPLICO B VECES EL RACIONAL A POR SI MISMO
        a_clon = racional_multiplicar(potencia, a);
        racional_destruir(potencia);
        potencia = a_clon;
    }//una vez que salgo del for tengo el resultado en potencia
    
    destruir_clones(numerador_a_clon, denominador_a_clon, numerador_b_clon, denominador_b_clon);

    if(racional_es_negativo(b)){
        racional_t *potencia_inversa = calculadora_inverso(potencia);
        racional_destruir(potencia);
        return potencia_inversa;
    }
    
    return potencia;
}







racional_t *resultado_computo_postfijo(cola_t *cola_postfijo){//como parametro mandariamos cola_de_salida
    //SI ES NUMERO APILO, SI ES OPERADOR O FUNCION DESAPILO TANTOS COMO NECESITE Y APILO EL RESULTADO PARCIAL
    pila_t *pila_aux = pila_crear(); //esta pila va a ser distinta a las demas, va a ser de racional_t
    if(pila_aux == NULL) return NULL;

    struct simbolo *aux; //auxiliar para ir sacando los elementos de la cola
    racional_t **numeros_desapilados = malloc(sizeof(racional_t *) * MAX_CANT_PARAMETROS); //como maximo voy a desapilar dos parametros por vez
    if(numeros_desapilados == NULL) return NULL;

    while((aux=cola_desencolar(cola_postfijo)) != NULL){
        if(aux->t == ENTERO || aux->t == RACIONAL){
            if(!pila_apilar(pila_aux, simbolo_a_racional(aux))){
                free_all(NULL, pila_aux, aux);
                // no libera racionales_desapilados (arreglo_racionales_a_desapilar)
                return NULL;
            }
        }

        else if(aux->t == FUNCION && aux->indice > INV){ //dsps de INV pasa a PI, PHI & EULER
            // lo modifico y lo hago un racional
            strcpy(aux->s, constantes[aux->indice]);
            aux->t = RACIONAL;
            aux->indice = (size_t) - 1; //si aux->t = RACIONAL => aux->indice no es válido por el I.R. Hago que tenga un valor imposible de ser en un indice real
            aux->n = DIGITOS_CONSTANTES;
            racional_t *irracional = simbolo_a_racional(aux); // un irracional que es racional, que gracioso...
            if(irracional == NULL){
                free_all(NULL, pila_aux, aux);
                destruir_numeros_desapilados(numeros_desapilados, MAX_CANT_PARAMETROS);
                return NULL;
            }
            if(!pila_apilar(pila_aux, irracional)){
                free_all(NULL, pila_aux, aux);
                destruir_numeros_desapilados(numeros_desapilados, MAX_CANT_PARAMETROS);
                return NULL;
            }
        }

        else if(aux->t == OPERADOR || aux->t == FUNCION){
            size_t cantidad_a_desapilar;
            if(aux->t == OPERADOR) 
                cantidad_a_desapilar = operador_aridad[aux->indice];
            else
                cantidad_a_desapilar = funcion_parametros[aux->indice];
            
            size_t i; 
            for(i = 0; i < cantidad_a_desapilar; i++){//caso en el que la cantidad de parametros sea distinta de 0
                numeros_desapilados[i] = pila_desapilar(pila_aux);
                if(numeros_desapilados[i] == NULL){
                    destruir_numeros_desapilados(numeros_desapilados, MAX_CANT_PARAMETROS); //cCAMBIE ESTO
                    fprintf(stderr, "CANTIDAD INVALIDA DE PARAMETROS");
                    return NULL;
                }
            }//aca ya me arme el arreglo con los racionales a computar
            

            // if(i < MAX_CANT_PARAMETROS){ //completo el arreglo que cree al principio (por default con MAX_CANT_PARAMETROS)
            //     for(size_t j = i; j < MAX_CANT_PARAMETROS;j++)
            //         numeros_desapilados[j] = NULL; 
            // }


            //HASTA ACA TENGO EL ARREGLO DE RACIONALES LISTO PARA COMPUTAR EL RESULTADO PARCIAL
            racional_t *resultado_parcial = operacion(aux, numeros_desapilados);
            //tengo que liberar el arreglo de racionales
            
            //destruir_numeros_desapilados(numeros_desapilados, MAX_CANT_PARAMETROS);
            for(size_t k = 0; k < cantidad_a_desapilar; k++)
                racional_destruir(numeros_desapilados[k]);
           
            if(!pila_apilar(pila_aux, resultado_parcial)){ //apilo el resultado y sigo con el while
                free(numeros_desapilados); 
                free_all(NULL, pila_aux, aux);
                racional_destruir(resultado_parcial);
                fprintf(stderr, "ERROR APILANDO Y COMPUTANDO EL POSTFIJO");
                return NULL;
            }        
        }
    }
    free(numeros_desapilados); 

    racional_t *resultado_final = pila_desapilar(pila_aux); 
    //si sigue habiendo elementos en la pila es porque hubo un error, chequeo eso

    if (!pila_esta_vacia(pila_aux)) {
        fprintf(stderr, "ERROR: La pila contiene más de un resultado al finalizar el cómputo (expresión mal formada)\n");
        racional_destruir(resultado_final);
        free_all(NULL, pila_aux, NULL);  // Limpio lo que queda, el aux se supone que termino en NULL
        return NULL;
    }
    free_all(NULL, pila_aux, NULL);
    return resultado_final;
    
}
