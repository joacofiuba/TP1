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

#include "ej1.h" //uso la funcion para pasar de binario a bcd


#define MAX_LINEA 256
#define CADENA_CORTA 8
#define CANT_OPERADORES 6
#define CANT_FUNCIONES 6
#define MAX_CANT_PARAMETROS 2


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
    size_t n; //solo es util para los numeros (ENTERO O RACIONAL)
    enum tipo t;
    size_t indice; // solo es util para simbolos de tipo OPERADOR o FUNCION!
 };




racional_t *calculadora_cambio_signo(racional_t *a){
    return a;
}
racional_t *calculadora_factorial(racional_t *a){
    return a;
}
racional_t *calculadora_absoluto(racional_t *a){
    return a;
}
racional_t *calculadora_inverso(racional_t *a){
    return a;
}
racional_t *calculadora_pi(){
    return NULL;
}
racional_t *calculadora_phi(){
    return NULL;
}
racional_t *calculadora_euler(){
    return NULL;
}

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
                aux->t = RACIONAL;
                while(isdigit(linea[i])){
                    aux->s[j++] = linea[i++];
                    n++;
                }
            }

            aux->s[j] = '\0';
            aux->n = n;
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
                cola_destruir(entrada, free); 
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
                cola_destruir(entrada, free); //TERMINAR
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
            cola_destruir(entrada, free); 
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

    if(numero->t == RACIONAL){
        
        while(i>0 && numero->s[i] == '0') i--; //ignoro los ceros no significativos
    
        while (i > 0 && numero->s[i] != '.') { //leo solo los decimales
            bcd[j++] = numero->s[(i--) - 1] - '0';
            cant_decimales++;
        }
        if (i > 0 && numero->s[i - 1] == '.') i--;
        while (i > 0) { //leo la parte entera
            bcd[j++] = numero->s[(i--)-1] - '0';
        }//j tiene la cantidad de digitos del arreglo bcd
    }

    else if(numero->t == ENTERO){
        while(i > 0){
            bcd[j++] = numero->s[(i--) - 1] - '0';
        }//j tiene la cantidad de digitos del arreglo
        
        
    }
    entero_t *den = NULL;
    if(cant_decimales != 0) {
        char bcd_diez[] = {0, 1};
        entero_t *entero_diez = entero_desde_bcd(bcd_diez, 2);
        if (!entero_diez) return NULL;

        entero_t *den = entero_clonar(entero_diez);
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

//INVERSO MULTIPLICATIVO



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
    
    entero_t *numerador_b = entero_clonar(racional_numerador(b));
    entero_t *denominador_b = entero_clonar(racional_denominador(b));

    entero_t *numerador_a = entero_clonar(racional_numerador(a));
    entero_t *denominador_a = entero_clonar(racional_denominador(a)); //hay que liberarlos

    //el racional b tiene que ser entero si o si
    if(entero_comparar(uno, racional_denominador(b)) != 0){
        fprintf(stderr, "PARA LA POTENCIA A^B, B TIENE QUE SER UN NUMERO ENTERO'\n");
        destruir_clones(numerador_a, denominador_a, numerador_b, denominador_b);
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
        destruir_clones(numerador_a, denominador_a, numerador_b, denominador_b);
        return potencia;
    }
    
    //CASO EN EL QUE B SEA 1 --> DEVUELVO a

    else if(entero_comparar(uno, racional_numerador(b)) == 0 && entero_comparar(uno,racional_denominador(b)) == 0){
        racional_t *potencia = racional_crear(racional_es_negativo(a), numerador_a, denominador_a);
        destruir_clones(numerador_a, denominador_a, numerador_b, denominador_b);
        entero_destruir(uno);
        entero_destruir(cero);
        return potencia;
    }



    //CASO EN EL QUE B SEA UN ENTERO NEGATIVO
    else if(!racional_es_negativo(b)){ //me tengo que crear un racional igual a b pero en positivo --> computo a^b y hago el inverso multiplicativo
        racional_t *b_clon_positivo = racional_crear(false, numerador_b, denominador_b);
        racional_t *potencia_positiva = calculadora_potencia(a, b_clon_positivo);
        racional_destruir(b_clon_positivo);
        entero_destruir(uno);
        entero_destruir(cero);
        destruir_clones(numerador_a, denominador_a, numerador_b, denominador_b);
        
        return calculadora_inverso(potencia_positiva);
    }

    entero_destruir(uno);
    entero_destruir(cero);
    //CASO EN EL QUE B SEA UN ENTERO POSITIVO
    //tengo que pasar a size_t el b 
    //numerador de b
    //convierto el numerador a bcd y de bcd a binario

    //entero_t *numerador_b = racional_numerador(b);
    
    size_t n;
    char *bcd_numerador_b = entero_a_bcd(racional_numerador(b), &n);
    if(n >= 10){
        fprintf(stderr, "LA POTENCIA SE VA DE RANGO, MAXIMO UN NUMERO DE 9 DIGITOS\n");
        return NULL;
    }
    size_t numerador_b_en_binario = bcd_a_binario(bcd_numerador_b, n);
    free(bcd_numerador_b);
    
    racional_t *a_clon = racional_crear(racional_es_negativo(a), numerador_a, denominador_a);
    racional_t *potencia = a_clon;
    
    for(size_t i = 1; i < numerador_b_en_binario; i++){//MULTIPLICO B VECES EL RACIONAL A POR SI MISMO
        a_clon = racional_multiplicar(potencia, a);
        racional_destruir(potencia);
        potencia = a_clon;
    }//una vez que salgo del for tengo el resultado en potencia
    
    destruir_clones(numerador_a, denominador_a, numerador_b, denominador_b);
    
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


void destruir_arreglo_racionales(racional_t **racionales, size_t cantidad){
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
    racional_t **arreglo_racionales_a_desapilar = malloc(sizeof(racional_t *) * MAX_CANT_PARAMETROS); //como maximo voy a desapilar dos parametros por vez
    if(arreglo_racionales_a_desapilar == NULL) return NULL;

    while((aux=cola_desencolar(cola_postfijo)) != NULL){
        if(aux->t == ENTERO || aux->t == RACIONAL){
            if(!pila_apilar(pila_aux, simbolo_a_racional(aux))){
                free_all(NULL, pila_aux, aux);
                return NULL;
            }
        }
        else if(aux->t == OPERADOR || aux->t == FUNCION){
            size_t cant_elementos_a_desapilar;
            if(aux->t == OPERADOR) cant_elementos_a_desapilar = operador_aridad[aux->indice];
            else
                cant_elementos_a_desapilar = funcion_parametros[aux->indice];
            
            size_t i; //A ESTE FOR ENTRA SOLO SI CANT_ELEMENTOS != 0
            for(i = 0; i < cant_elementos_a_desapilar; i++){//caso en el que la cantidad de parametros sea distinta de 0
                arreglo_racionales_a_desapilar[i] = pila_desapilar(pila_aux);
                if(arreglo_racionales_a_desapilar[i] == NULL){
                    fprintf(stderr, "CANTIDAD INVALIDA DE PARAMETROS");
                    return NULL;
                }
            }//aca ya me arme el arreglo con los racionales a computar
            if(i < MAX_CANT_PARAMETROS){ //completo el arreglo que cree al principio (por default con MAX_CANT_PARAMETROS)
                for(size_t j = i; j < MAX_CANT_PARAMETROS;j++)
                    arreglo_racionales_a_desapilar[j] = NULL; 
            }
            //HASTA ACA TENGO EL ARREGLO DE RACIONALES LISTO PARA COMPUTAR EL RESULTADO PARCIAL
            racional_t *resultado_parcial = operacion(aux, arreglo_racionales_a_desapilar);
            //tengo que liberar el arreglo de racionales
            destruir_arreglo_racionales(arreglo_racionales_a_desapilar, MAX_CANT_PARAMETROS);
            if(!pila_apilar(pila_aux, resultado_parcial)){
                free_all(NULL, pila_aux, aux);
                racional_destruir(resultado_parcial);
                fprintf(stderr, "ERROR APILANDO Y COMPUTANDO EL POSTFIJO");
                return NULL;
            }//apilo el resultado y sigo con el while
        }
    }
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
        free(elemento_cola_salida);
    }

    cola_destruir(cola_infija, free);

    cola_destruir(cola_postfija, free);

    printf("\n");
    return 0;
}
