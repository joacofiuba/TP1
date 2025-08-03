Leeme!

El repositorio debe contener 

    * dir: /tdas
    * archivos: main.c Makefile README.md 
    * provisionalmente también ej1.h 

Por favor asegurarse de no añadir nada por fuera de eso al usar "git add"

Para evitar subir los .o (codigo objeto) antes de usar "$ git add --all", 
usar el comando "$ make clean"



# Nota a lautaro:  

`struct simbolo` es nuestro pseudo tda, tiene destructor pero es al pedo. 
podriamos liberar memoria usando free(datp), siendo dato el nombre
que tenga el puntero a struct simbolo en ese momento. 

esto es porque struct simbolo no tiene miembros que utilicen memoria 
dinamica, por lo que no hay que pensar una forma especial de destruirlos.

no lo saqué porque capaz estoy obviando algo y existe un motivo, pero en
caso de que no y lo consideres más limpio, eliminalo. en caso de que haya
que dejarlo por favor cambiale el nombre a `simbolo_destruir()`

respetemos que las "primitivas" de struct simbolo empiecen con el 'simbolo'
al inicio como en el TP2 y TP3 (entero_... , racional_...).



# Sobre los cambios que hice: 

1. agregué dos tablas de busqueda para tener un 
indice único como miembro de estructura. básicamente el indice coincide
con su cadena y aridad/parametros (dependiendo de si es operador/función
respectivamente). por lo tanto, con saber el indice sabes todos esos datos
automaticamente

esto trae un inconveniente, simbolo->indice no se inicializa cuando el tipo
no es OPERADOR o FUNCION. la solución es plantear eso como invariante, 
es mejor que antes donde no se inicializaban directamente 3 variables y 
la lógica en la postfija se complicaba bastante.

2. puse #define para los tamaños de los enumerativos,
`#define CANT_OPERADORES 6`
`#define CANT_FUNCIONES 6`

así elimine los enumerativos que habian al final que eran medio raros,
en la función de busqueda genérica se usa como parametro cantidad esos 
define

3. Cambié el nombre de la función `limpiar_todo()` por `free_all()`

4. Solucioné las fugas de memoria, el problema principal estaba en no 
destruir los simbolos al ir desencolando para imprimir en el main.
Ojo con eso, si no destruis o guardas el puntero al desencolar perdes
la memoria. Creo que igual lo sabias pq en la postfija no hay fugas
y se desapila/desencola todo el rato. 
