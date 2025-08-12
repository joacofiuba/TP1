#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>   
#include <math.h>

#include "tdas/cola.h"
#include "tdas/pila.h"
#include "tdas/racional.h"
#include "tdas/entero.h"

#include "tdas/ej1.h" //uso la funcion para pasar de binario a bcd


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

const char *constantes[] = {
    [PI] = "3.1416",
    [EULER] = "2.7182",
    [PHI] = "1.6180"
};

// Invariante: 0 <= t <= 5
// indice se inicializa <=>  tipo == OPERADOR || FUNCION
struct simbolo {
    char s[MAX_LINEA];
    size_t n; //solo es util para los numeros (ENTERO O RACIONAL)
    enum tipo t;
    size_t indice; // solo es util para simbolos de tipo OPERADOR o FUNCION!
 };




// busqueda generica
size_t cadena_a_enumerativo(const char *s, const char *opciones[], size_t cantidad){ //en cantidad mandarias CANT_OPERADORES o CANT_FUNCIONES
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(s, opciones[i])){
            return i;
        }
    }
    return cantidad + 1; // devuelve un valor fuera del rango del indice, indicando que no se encuentra en la tabla de busqueda
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
        size_t n=0; //contador para la la longitud de la cadena
        if (isdigit(linea[i])) {
            aux->t = ENTERO;
            aux->s[j++] = linea[i++];
            n++;
            while(isdigit(linea[i])) {
                aux->s[j++] = linea[i++];
                n++;
            }

            // Detectar racional (con punto)
            if (linea[i] == '.' && isdigit(linea[i + 1])) {
                aux->s[j++] = linea[i++]; // incluir el punto
                n++;
                aux->t = RACIONAL;
                while(isdigit(linea[i])){
                    aux->s[j++] = linea[i++];
                    n++;
                }
            }

            aux->s[j] = '\0';
            aux->n = n;
            if(!cola_encolar(entrada, aux)){
                cola_destruir(entrada, free);
                free(aux);
                return NULL;
            }
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
                cola_destruir(entrada, free); 
                return NULL;
            }

            aux->indice = indice_funcion;
            if(!cola_encolar(entrada, aux)){
                cola_destruir(entrada, free);
                free(aux);
                return NULL;
            }
            continue;
        }


        // Paréntesis
        if(linea[i] == '('){
            balance_parentesis++;
            aux->t = PARENTESIS_ABIERTO;
            aux->s[j++] = linea[i++];
            aux->s[j] = '\0';
            if(!cola_encolar(entrada, aux)){
                cola_destruir(entrada, free);
                free(aux);
                return NULL;
            }
            continue;
        }
        if(linea[i] == ')'){
            balance_parentesis--;
            if(balance_parentesis < 0){
                fprintf(stderr, "¡Se está cerrando un paréntesis que nunca se abrió!");
                free(aux);
                cola_destruir(entrada, free); 
                return NULL;
            }
            aux->t = PARENTESIS_CERRADO;
            aux->s[j++] = linea[i++];
            aux->s[j] = '\0';
            if(!cola_encolar(entrada, aux)){
                cola_destruir(entrada, free);
                free(aux);
                return NULL;
            }
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
            cola_destruir(entrada, free); 
            return NULL;
        }

        // if(indice_operador >= SUMA && indice_operador <= POTENCIA) aux->aridad = 2; // NO DEBERAI SER NECESARIO JEJEJE
        // else if(indice_operador == CAMBIO_SIGNO) aux->aridad = 1;
        
        aux->indice = indice_operador; 
        if(!cola_encolar(entrada, aux)){
            cola_destruir(entrada, free);
            free(aux);
            return NULL;
        }

    } //aca termina el while principal

     //Si no se cierra el parentesis
    if(balance_parentesis != 0){
        fprintf(stderr, "Falta cerrar un paréntesis \n");
        cola_destruir(entrada, free);
        return NULL;
    }

    return entrada;
}

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


cola_t *cola_de_salida(cola_t *entrada){ //se devuelve la cola con notacion postfija (shunting yard)
    cola_t *salida = cola_crear();
    if(salida == NULL) return NULL;

    pila_t *pila = pila_crear();
    if(pila == NULL){
        cola_destruir(salida, free);
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
                    free(pila_desapilar(pila)); //descarto el (
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
    pila_destruir(pila, free);

    return salida;

}

//FUNCION AUXILIAR PARA CREAR NUMEROS RACIONALES
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


//FUNCIONES DE OPERACIONES Y FUNCIONES

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
//aclarar en struct simbolo que el n incluye el . en los digitos
racional_t *calculadora_inverso(const racional_t *a){
    
    entero_t *numerador = entero_clonar(racional_numerador(a));
    entero_t *denominador = entero_clonar(racional_denominador(a));
    racional_t *inverso = racional_crear(racional_es_negativo(a), denominador, numerador);
    entero_destruir(numerador);
    entero_destruir(denominador);
    return inverso;
}

//POTENCIA

//FUNCION AUXILIAR PARA DESTRUIR LOS NUMERADORES Y DENOMINADORES
void destruir_clones(entero_t *num_a, entero_t *den_a, entero_t* num_b, entero_t *den_b){
    entero_destruir(num_a);
    entero_destruir(den_a);
    entero_destruir(num_b);
    entero_destruir(den_b);
}
//DESPUES DE USAR ESTA FUNCION HAY QUE LIBERAR LOS RACIONALES A Y B
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


//COMPUTO DEL POSTFIJO


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
    free(numeros_desapilados); //CAMBIE ESTO

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
//FUNCIONES AUXILIARES PARA EL MAIN 

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
    
        if(cola_infija == NULL) return 1; // podriamos agregar esto --> printf("Error al armar cola de entrada\n");

        cola_t *cola_postfija = cola_de_salida(cola_infija);
        if(cola_postfija == NULL){
            cola_destruir(cola_infija, free);
            return 1;
        }
        cola_destruir(cola_infija, free);

        racional_t *r = resultado_computo_postfijo(cola_postfija);
        if(r == NULL){
            cola_destruir(cola_postfija, free);
            return 1;
        }
        
        cola_destruir(cola_postfija, free);
        //
        
        //hasta aca tengo el resultado en modo racional
        if(en_modo_racional){
            if(!racional_imprimir(r)){
                racional_destruir(r);
                fprintf(stderr, "Error imprimiendo en modo racional\n");
                return 1;
            }
        }
        else{
            if(!imprimir_decimales(r, decimales)){
                racional_destruir(r);
                fprintf(stderr, "Error imprimiendo en modo decimal\n");
                return 1;
            }
        }
            
        racional_destruir(r);
        printf("\n");
        

    }

    return 0;
}

