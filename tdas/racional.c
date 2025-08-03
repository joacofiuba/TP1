#include "racional.h"
#include "entero.h"
#include <stdio.h>
#include <stdlib.h>

struct racional {
    // Representa una fracción mixta de la forma (-1)^s * (n / d).
    // s es el signo, n el numerador y d el denominador.
    // Invariante de representación:
    // *) n y d diferentes de NULL.
    // *) s será false o true según el número sea positivo o negativo respetivamente.
    // *) La fracción es irreducible, es decir n y d son coprimos.
    // *) d >= 1 (si n != 0).
    // *) Si n == 0 => d == 1 && s == false.
    bool s;
    entero_t *n, *d;
};

static entero_t *max_comun_divisor(const entero_t *a, const entero_t *b){
    entero_t *m = entero_clonar(a);
    entero_t *n = entero_clonar(b);
    entero_t *r; 
    entero_t *cero = entero_cero();

    while(1){//entero_comparar(n, cero) != 0){
        entero_dividir(m, n, &r);
        entero_destruir(m);
        if(entero_comparar(r, cero) == 0)
            break;
        m = n;
        n = r; 
    }
    entero_destruir(r);
    entero_destruir(cero);

    return n;
}

static bool simplificar_fraccion(entero_t *n, entero_t *d){
    // Chequeo si denominador == 1
    entero_t *uno = entero_uno();
    if(entero_comparar(d, uno) == 0){ 
        entero_destruir(uno);
        return false;
    }
    
    entero_t *m = max_comun_divisor(n, d);
    
    // Chequeo si la fracción es irreductible
    if(entero_comparar(m, uno) == 0){
        entero_destruir(uno); 
        entero_destruir(m);
        return false;
    }
    
    // Simplifico
    entero_dividir(n, m, NULL);
    entero_dividir(d, m, NULL);

    entero_destruir(uno);
    entero_destruir(m);

    return true;
}


// Constructor
racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador){
    entero_t *n, *d;

    racional_t *q = malloc(sizeof(racional_t));
    if(q == NULL) return NULL;
    
    entero_t *cero = entero_cero();

    // i.r.: Si n == 0 => d == 1 &&  s = false
    if(entero_comparar(numerador, cero) == 0){
        q->n = entero_cero();
        q->d = entero_uno();
        q->s = false;
        entero_destruir(cero);
        return q;
    }
    
    n = entero_clonar(numerador);
    if(denominador == NULL)
        d = entero_uno();
    else{ 
        d = entero_clonar(denominador); 
        simplificar_fraccion(n, d);
    }
    
    q->n = n;
    q->d = d;
    q->s = es_negativo ? 1 : 0;   
    
    entero_destruir(cero);
    return q;
}

// Destructor
void racional_destruir(racional_t *q){
    entero_destruir(q->n);
    entero_destruir(q->d);
    free(q);
}

const entero_t *racional_numerador(const racional_t *q){
    return q->n; 
}

const entero_t *racional_denominador(const racional_t *q){
    return q->d; 
}

bool racional_es_negativo(const racional_t *q){
    return q->s;
}

bool racional_imprimir(const racional_t *q){
    q->s ? printf("-") : printf("+");
    entero_imprimir(q->n);

    entero_t *uno = entero_uno();
    if(entero_comparar(q->d, uno) != 0){
        printf("/");
        entero_imprimir(q->d);
    }
    entero_destruir(uno); 

    return true;
}

racional_t *racional_sumar(const racional_t *q, const racional_t *r){
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);
    bool es_negativo; 

    bool signo_distinto = q->s ^ r->s; 
    // Construyo el numerador de la suma, lo guardo en aux1
    entero_multiplicar(aux1, r->d);
    entero_multiplicar(aux2, r->n);

    if(signo_distinto){
        int cmp = entero_comparar(aux1, aux2);
        if(cmp == 0){
            // q == s
            entero_destruir(aux1);
            entero_destruir(aux2);
            entero_t *cero = entero_cero();
            entero_t *uno = entero_uno();
            racional_t *suma = racional_crear(false, cero, uno);
            entero_destruir(cero);
            entero_destruir(uno);
            return suma;
        }

        // La suma da positiva o negativa, depende del signo del mayor
        else if(cmp > 0){
            // q > r
            es_negativo = q->s ? true : false;
            entero_restar(aux1, aux2);
        }
        else if(cmp < 0){
            // q < r 
            es_negativo = r->s ? true : false;
            entero_restar(aux2, aux1);
            entero_t *aux3 = aux1;
            aux1 = aux2;
            aux2 = aux3;
        } 
    }
    else { // mismo signo
        es_negativo = q->s ? true : false;
        entero_sumar(aux1, aux2);
    }
    
    entero_destruir(aux2);
    
    // Denominador de la suma, lo guardo en aux2
    aux2 = entero_clonar(q->d);
    entero_multiplicar(aux2, r->d);

    simplificar_fraccion(aux1, aux2);
    racional_t *suma = racional_crear(es_negativo, aux1, aux2);

    entero_destruir(aux1);
    entero_destruir(aux2);

    return suma;
}

racional_t *racional_restar(const racional_t *q, const racional_t *r){
    racional_t *aux = racional_crear(1 ^ r->s, r->n, r->d);
    racional_t *resta = racional_sumar(q, aux);
    racional_destruir(aux);
    return resta;
}

racional_t *racional_multiplicar(const racional_t *q, const racional_t *r){
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);

    entero_multiplicar(aux1, r->n);
    entero_multiplicar(aux2, r->d);

    simplificar_fraccion(aux1, aux2);
    
    // Podria crear una función privada que utilice los enteros
    // que le pase para crear un racional, en vez de clonarlos 
    racional_t *producto = malloc(sizeof(racional_t));
    if(producto == NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        return NULL;
    }
    
    producto->n = aux1;
    producto->d = aux2;
    producto->s = r->s ^ q->s;

    return producto;
}

racional_t *racional_dividir(const racional_t *q, const racional_t *r){
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);
    
    entero_multiplicar(aux1, r->d);
    entero_multiplicar(aux2, r->n);
    
    simplificar_fraccion(aux1, aux2);

    racional_t *division = malloc(sizeof(racional_t));
    if(division == NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        return NULL;
    }
    
    division->n = aux1;
    division->d = aux2;
    division->s = r->s ^ q->s;

    return division;
}
