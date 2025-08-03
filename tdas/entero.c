#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "entero.h"

#define MASK_LSB 0x01
#define SHIFT_MSB 31
#define MASK_MSB 0x80000000
#define UINT33 0x0100000000
#define BRW_ON  0x0100000000
#define BRW_OFF 0x0000000000
#define DIGITOS_BIN 3.321928095

struct entero {
    uint32_t *d;
    size_t n;
};

// Devuelve el la cantidad de digitos del entero
// En base 2^32. Al redimensionar usando 
// corte(e) se obtiene un entero sin ceros al final. 
static size_t corte(entero_t *e){
    bool flag = false;
    size_t corte;

    for(size_t i = 0; i < e->n; i++){
        if(e->d[i] == 0 && flag == false){
            corte = i;
            flag = true;
        }

        else if(e->d[i] != 0){
            corte = i;
            flag = false;
        }
    }

    return corte + 1;
}

entero_t *entero_cero(void){
    entero_t *e = malloc(sizeof(entero_t));
    if(e == NULL) return NULL;

    e->d = malloc(1 * sizeof(uint32_t));
    if(e->d == NULL){
        free(e);
        return NULL;
    } 

    e->d[0] = 0;
    e->n = 1;

    return e;
}

entero_t *entero_uno(void){
    entero_t *e = malloc(sizeof(entero_t));
    if(e == NULL) return NULL;

    e->d = malloc(1 * sizeof(uint32_t));
    if(e->d == NULL){
        free(e);
        return NULL;
    } 

    e->d[0] = 1;
    e->n = 1;

    return e;

}

static entero_t *_crear(size_t n){ 
    entero_t *entero = malloc(sizeof(entero_t));
    if(entero == NULL) return NULL;

    entero->d = malloc(n * sizeof(uint32_t));
    if(entero->d == NULL){
        free(entero);
        return NULL; 
    } 
    for(size_t i = 0; i < n; i++)
        entero->d[i] = 0;

    entero->n = n;
    return entero;
}

void entero_destruir(entero_t *entero){
    free(entero->d);
    free(entero);
}

entero_t *entero_clonar(const entero_t *entero){
    size_t n = entero->n;
    entero_t *aux = _crear(n);
    if(aux == NULL) return NULL;

    for(size_t i = 0; i < n; i++)
        aux->d[i] = entero->d[i];
    return aux;
}

// realloc de d[n] => d[N], N > n.
// d[0<i<n] == entero_original, d[n<i<N] = 0 
// d[N] = d([0,n]U[n+1, N]) = d([orig]U[N-n ceros]) == entero redimensionado
static bool _redimensionar(entero_t *entero, size_t n){
    uint32_t *aux = realloc(entero->d, n * sizeof(uint32_t)); 
    if(aux == NULL) 
        return false;

    for(size_t i = entero->n; i < n; i++)
        aux[i] = 0;

    entero->d = aux;
    entero->n = n;
    return true;
}

int entero_comparar(const entero_t *a, const entero_t *b){
    size_t na = a->n;
    size_t nb = b->n;
    if(na > nb) return 1;
    if(na < nb) return -1;

    size_t i = na;
    while(i > 0){
        i--;
        if(a->d[i] != b->d[i]){
            if(a->d[i] > b->d[i]) return 1;
            return -1;
        }	
    }
    return 0;
}


bool entero_desplazar_derecha(entero_t *e){
    uint32_t msb_sig = 0;
    // uso signed int64_t pq con size_t tengo overflow cuando i < 0
    // se podría usar también en la función entero_comparar() y usar 
    // for en vez de while
    for(int64_t i = e->n - 1; i >= 0; i--){
        uint32_t lsb = e->d[i] & MASK_LSB;
        e->d[i] >>= 1;
        e->d[i] |= msb_sig << SHIFT_MSB;
        msb_sig = lsb;
    }
    return true;
}

bool entero_desplazar_izquierda(entero_t *e){
    uint32_t lsb_sig = 0;
    size_t i;
    for(i = 0; i < e->n; i++){
        uint32_t msb = (e->d[i] & MASK_MSB) >> SHIFT_MSB;
        e->d[i] <<= 1;
        e->d[i] |= lsb_sig;
        lsb_sig = msb;
    }

    if(lsb_sig == 1){
        if(!_redimensionar(e, e->n + 1)) return false;
        e->d[i] |= lsb_sig;
    }

    return true;
}


bool entero_sumar(entero_t *a, const entero_t *b){
    size_t na = a->n, nb = b->n, n, nf; 
    const entero_t *p;
    
    if(na >= nb){
        p = a;
        nf = na, n = nb;
    }
    else{ // nb mayor
        p = b;
        nf = nb; n = na;
        if(!_redimensionar(a, nb)) return false; 
    }

    char c = 0; //carry
    size_t i;
    uint64_t suma = 0; 
    for(i = 0; i < nf; i++){ 
        // 0 < i < n -> sumo elemento a elemento de a y b y chequeo si existe carry
        if(i < n){
            suma = (uint64_t)a->d[i] + b->d[i] + c;
            if(suma < UINT33){
                a->d[i] = suma;
                c = 0;
            }
            else{
                a->d[i]= suma - UINT33;
                c = 1;
            }
            continue;
        }
        if(c){
            // n < i < nf => a o b fueron recorridos por completo
            // queda sumar los elementos restantes de a o b + carry
            // hasta que deje de haber carry
            suma = (uint64_t)p->d[i] + c; // si d[i] = 0xFFFFFFFF => 32bits => d[i]+carry desborda
            if(suma <= UINT33){
                a->d[i] = suma;
                c = 0;
            }
            else{
                a->d[i]= suma - UINT33;
                c = 1;
            }
            continue;
        }
        break;
    }

    if(c){ // último chequeo de carry, si hay carry el vector es de nf + 1 elementos
        if(!_redimensionar(a, i + 1)) return false;
        a->d[i] = 1;
        return true;
    }

    if(nf == nb) // b > a => copia los elementos restantes de b en a
        for(i = i; i < nf; i++)
            a->d[i] = b->d[i];

    return true; 
}


bool entero_restar(entero_t *a, const entero_t *b){
    size_t na = a->n, nb = b->n; 
    if(entero_comparar(a, b) < 0) return false;

    uint64_t brw = 0; //borrow
    size_t i;
    uint64_t resta = 0;
    char hubo_brw = 0;
    for(i = 0; i < na; i++){
        if(i < nb){
            if((int32_t)(a->d[i] - hubo_brw) < b->d[i])
                brw = BRW_ON;
            else 
                brw = BRW_OFF;
            resta = brw + a->d[i] - hubo_brw - b->d[i] ;
            a->d[i] = resta;
            // Si húbo borrow => tengo que restar ese 1 que pedi prestado al digito que sigue
            if(brw)
                hubo_brw = 1;
            else 
                hubo_brw = 0;
            continue;
        }
        // Misma lógica pero para los digitos que quedan de a->d[]
        if(a->d[i] - hubo_brw < 0)
            brw = BRW_ON;
        else
            brw = BRW_OFF;
        a->d[i] = a->d[i] + brw - hubo_brw;
        if(brw)
            hubo_brw = 1;
        else 
            break;
    }
    return true;
}

// ej//						n_bin * 3
//  -------------------------------------------------------
// |  8bcd |  8bcd   |  8bcd   |  8bcd   |   bin |    bin   |
// ---------------------------------------------------------
//    24 digitos bcd   máx representacion binaria = 20 digitos decimales

char *entero_a_bcd(const entero_t *entero, size_t *n){
    size_t n_bin = entero->n;

    entero_t *aux = entero_clonar(entero);
    if(aux == NULL) return NULL;
    // Podría ser menos (2.5), pero complica excesivamente la lógica de los corrimientos a izquierda.
    if(_redimensionar(aux, 3 * n_bin) == false){
        free(aux);
        return NULL;	
    } 

    size_t n_aux = aux->n;
    for(int i = 0; i < 32 * n_bin; i++) { 
        for(size_t j = 0; j < (n_aux - n_bin) ; j++){
            // Chequear que ningún BCD valga más de 4, si no, sumarle 3
            for(int d = 0; d < 8 * 4 ; d += 4){
                uint64_t digito = (aux->d[j + n_bin] >> d) & 0xF; 
                if(digito > 4){
                    digito += 3;
                    aux->d[n_bin + j] &= ~(0xFL << d);
                    aux->d[n_bin + j] |= digito << d;
                }
            }
        }
        entero_desplazar_izquierda(aux);
    }

    char *bcd = malloc(sizeof(char) * ((n_aux - n_bin) * 8 + 8)); 
    if(bcd == NULL){
        free(aux);
        return NULL;
    }	

    *n = 0; 
    // Variable que corta el tamaño del bcd (sirve para determinar el largo del bcd[])
    bool corte = 0;


    // Recorro los elementos reservados dentro del aux para los digitos bcd y los guardo en el vector bcd[].
    // Lo trabajo como una matriz, si j++ => paso a bcd el siguiente bloque de 4 bytes de aux.
    for(size_t j = 0; j < (n_aux - n_bin); j++){
        for(size_t i = 0; i < 8; i++){ 
            char digito = (aux->d[n_bin + j] >> (i * 4)) & 0xF;
            if(digito == 0 && corte == false){ 
                *n = j * 8 + i; // digitos del bcd
                corte = true;
            }	
            else if(digito != 0){
                corte = false;
            }
            bcd[j * 8 + i] = digito;
        }
    }

    entero_destruir(aux);

    return bcd;
}



/*
2^32^x = 10^n
n -> digitos
x -> digitos en base 2^32 necesarios para representar n digitos en base 10 
x = n * 1/32 * log(10)/log(2) == n * DIGITOS_BIN / 32
-----------------------------------------------------------------
|  LS  |     |     | MS | LS |     |     |     |     |     | MS  |
-----------------------------------------------------------------
         bin           |              bcd                        |
*/

entero_t *entero_desde_bcd(const char bcd[], size_t n){
    if(n == 0) n = 1; // fixea un bug del EJ3, se podría retornar NULL

    size_t n_bcd = ceil(n / 8.0);
    size_t n_bin = ceil(n * DIGITOS_BIN / 32.0); 
    size_t n_aux = n_bcd + n_bin;

    entero_t *aux = _crear(n_aux);
    if(aux == NULL) return NULL;

    for(int64_t j = n_bcd - 1; j >= 0; j--)
        for(int64_t i = 8 - 1; i >= 0; i--){
            if((j * 8 + i) >= n) continue;
            aux->d[n_bin + j] |= bcd[j * 8 + i] << (i * 4);
        }

    // Shifteo de más, para que el lsb me quede en aux->d[0]
    for(size_t j = 0; j < (32 * n_bin); j++){
        entero_desplazar_derecha(aux);
        for(size_t i = 0; i < n_bcd; i++){
            for(size_t d = 0; d < 8 * 4; d += 4){
                uint32_t digito = (aux->d[n_bin + i] >> d) & 0xF; 
                if(digito > 7){
                    digito -= 3;
                    aux->d[n_bin + i] &= ~(0xFUL << d);
                    aux->d[n_bin + i] |= digito << d;
                }
            }		
        }
    }
    
    aux->n = n_bin;
    if(_redimensionar(aux, corte(aux)) == false){
        free(aux);
        return NULL;
    }

    return aux;
}


static void imprimir_bcd(const char bcd[], size_t digitos){  
    size_t pos_primer_grupo = digitos % 3; 
    if(digitos == 0) {
        printf("0");
        return; 
    }
    for(size_t i = 0; i < digitos; i++){
        if(i != 0 && ((int)(i - pos_primer_grupo) % 3) == 0 && digitos > 2){
            printf("_");            
        }
        printf("%d", bcd[digitos - 1 - i]); 
    }
}   


bool entero_imprimir(const entero_t *entero){
    size_t n;
    char *bcd = entero_a_bcd(entero, &n);
    if(bcd == NULL) return false;
    imprimir_bcd(bcd, n);
    free(bcd);
    return true;
}


bool entero_dividir(entero_t *dividendo, const entero_t *divisor, entero_t **resto) {
    entero_t *d = entero_clonar(divisor);
    entero_t *r = entero_clonar(dividendo);
    size_t n = dividendo->n;
    if(d == NULL || r == NULL) {
        entero_destruir(d);
        entero_destruir(r);
        return false;
    }

    // Redimensiona dividendo y lo hace cero. 
    if(r->n != 1)
        if(_redimensionar(dividendo, 1) == false)
            return false; 
    dividendo->d[0] = 0;
    entero_t *q = dividendo;

    // Podríamos tener una función para desplazar elementos en el arreglo.
    for(size_t i = 0; i < n * 32; i++)
        entero_desplazar_izquierda(d);

    for(size_t i = 0; i < n * 32; i++) {
        entero_desplazar_izquierda(q);
        entero_desplazar_izquierda(r);
        if(entero_comparar(r, d) >= 0) {
            entero_restar(r, d);
            q->d[0] |= 1;
        }
    }

    entero_destruir(d);

    // Podríamos tener una función para desplazar elementos en el arreglo.
    for(size_t i = 0; i < n * 32; i++)
        entero_desplazar_derecha(r);

    if(resto != NULL){
        _redimensionar(r, corte(r));
        *resto = r;
        return true;
    }

    entero_destruir(r);
    return true;
}


bool entero_multiplicar(entero_t *a, const entero_t *b) {
    entero_t *aa = _crear(1);
    if(aa == NULL) return false;

    entero_t *aux = _crear(a->n + b->n + 1);
    if(aux == NULL) {
        entero_destruir(aa);
        return false;
    }

    // Swapeamos el contenido de a con aa, vamos a guardar el resultado en a
    uint32_t *swap = aa->d;
    aa->d = a->d;
    a->d = swap;
    aa->n = a->n;
    a->n = 1;

    for(size_t i = 0; i < b->n; i++) {
        for(size_t j = 0; j < aa->n; j++) {
            uint64_t m = (uint64_t)aa->d[j] * b->d[i];
            aux->d[j] = m;
            aux->d[j + 1] = m >> 32;

            entero_sumar(a, aux);

            aux->d[j] = 0;
        }
        aux->d[aa->n] = 0;

        // INSISTO con lo que dije en dividir:
        // Podríamos tener una función para desplazar elementos en el arreglo.
        for(size_t j = 0; j < 32; j++)
            entero_desplazar_izquierda(aa);
    }

    entero_destruir(aa);
    entero_destruir(aux);

    return true;
}


