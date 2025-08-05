#include <stdio.h>
#include <stdlib.h>
#define CANT_DIGITOS_A_SEPARAR 3
#define MAX_CADENA 100
#define OPERACION_SUMA '+'
#define OPERACION_MULTIPLICACION '*'
#define SEPARADOR '_'
void imprimir_bcd(const char bcd[], size_t digitos){
    for(size_t i = digitos;i>0;i--){
        printf("%d",bcd[i-1]);
        if(((i-1)%CANT_DIGITOS_A_SEPARAR)==0 && (i-1)!=0){
            putchar(SEPARADOR); 
        }
    }
}

unsigned long bcd_a_binario(const char bcd[], size_t digitos){
    unsigned long acc = 0;
    unsigned long potencia = 1;
    for(size_t i=0; i< digitos;i++){
        acc+= bcd[i] * potencia;
        potencia *=10;
    }
    return acc;
}

size_t binario_a_bcd(unsigned long binario, char bcd[]){
    unsigned long acc = binario;
    size_t i = 0;
    if( binario == 0){
        bcd[0] = 0;
        return 1;
    }
    for(i = 0; acc!=0; i++){
        bcd[i]= acc%10;
        acc = acc/10;
    }
    return i;

}

size_t sumar_bcd(const char a[], size_t na, const char b[], size_t nb, char r[]){
    char suma, carry = 0;
    size_t n_grande = na, n_chica = nb, i = 0;
    
    if(nb > na){
        n_grande = nb;
        n_chica = na;
    }

    for(i = 0; i<n_grande; i++){
        if(i<n_chica){
            suma = a[i] + b[i] + carry;
        }
        if(i >= n_chica){
            if(na > nb) suma = a[i] + carry;
            if(nb > na) suma = b[i] + carry;
        }
        carry = suma/10;
        r[i] = suma % 10;

    }
    if (carry != 0) r[i++] = 1; 
    

    return i;
}

size_t multiplicar_por_un_digito(const char a[], size_t na, char b, char r[]){
    size_t i = 0;
    char acc=0;
    char carry=0;
    for(i=0; i<na; i++){
        acc = (a[i]*b) + carry;
        if(acc>9){
            r[i] = acc%10;
            carry = acc/10;
        }
        else{
            r[i] = acc;
            carry = 0;
        }
    }
    if(carry != 0){
        r[i] = carry;
        i++;
    }
    return i;
}

void imprimir_operacion(const char bcd1[], size_t n1, const char bcd2[], size_t n2, const char resultado[], size_t n_res, char operacion) {
    imprimir_bcd(bcd1, n1);
    putchar(' ');
    putchar(operacion);
    putchar(' ');
    
    if (operacion == OPERACION_SUMA)
        imprimir_bcd(bcd2, n2);
    else if (operacion == OPERACION_MULTIPLICACION)
        printf("%d", bcd2[0]);

    printf(" = ");
    imprimir_bcd(resultado, n_res);
    putchar('\n');
}
/*
int main() {
    char entrada1[MAX_CADENA];
    char entrada2[MAX_CADENA];
    char bcd_entrada1[MAX_CADENA];
    char bcd_entrada2[MAX_CADENA];
    char resultado[MAX_CADENA];

    while (1) {
        fgets(entrada1, MAX_CADENA, stdin);
        fgets(entrada2, MAX_CADENA, stdin);

        unsigned long bin_entrada1 = atoi(entrada1);
        unsigned long bin_entrada2 = atoi(entrada2);

        size_t cant_dgts_ent1 = binario_a_bcd(bin_entrada1, bcd_entrada1);
        size_t cant_dgts_ent2 = binario_a_bcd(bin_entrada2, bcd_entrada2);

        //SUMA
        size_t cant_dgts_suma = sumar_bcd(bcd_entrada1, cant_dgts_ent1, bcd_entrada2, cant_dgts_ent2, resultado);
        imprimir_operacion(bcd_entrada1, cant_dgts_ent1, bcd_entrada2, cant_dgts_ent2, resultado, cant_dgts_suma, OPERACION_SUMA);

        //MULTIPLICACION por un dígito
        size_t cant_dgts_mult = multiplicar_por_un_digito(bcd_entrada1, cant_dgts_ent1,bcd_entrada2[0], resultado);
        imprimir_operacion(bcd_entrada1, cant_dgts_ent1, bcd_entrada2, cant_dgts_ent2, resultado, cant_dgts_mult, OPERACION_MULTIPLICACION);
    }

    return 0;
}

      
*/