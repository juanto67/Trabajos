/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "ficheros_basico.h"


/* ----------------------------------------
                 NIVEL 2
---------------------------------------- */

/*
Función que calcula el tamaño del mapa de bits, para conocer dicha información
haremos unas simples operaciones de división, sin embargo hay que comprobar mediante
el módulo si se necesita un bloque extra.

nbloques: bloques del dispositivo virtual

salida: los bloques necesarios para crear el MB

*/ 
int tamMB(unsigned int nbloques)
{  
    unsigned int bytes = (nbloques / 8); // Calculamos la cantidad de bytes necesarios para representar los bits
    unsigned int bloques = bytes / BLOCKSIZE; // Calculamos la cantidad de bloques necesarios para representar los bytes

    if (bytes % BLOCKSIZE != 0) { // Añadimos un bloque adicional si hay bytes restantes
        bloques++;
    }

    return bloques;
}
/*
Función que calcula el tamaño del array de inodos, para conocer dicha información
haremos unas simples operaciones de división, sin embargo hay que comprobar mediante
el módulo si se necesita un bloque extra.

ninodos: los inodos del dispositivo virtual

salida: los bloques necesarios para crear el AI

*/ 
int tamAI(unsigned int ninodos)
{ 
    unsigned int inodos = ninodos;
    unsigned int bloques = inodos / (BLOCKSIZE / INODOSIZE); // Calculamos la cantidad de bloques necesarios para los inodos

    if (inodos % (BLOCKSIZE / INODOSIZE) != 0) { // Añadimos un bloque adicional si hay inodos restantes
        bloques++;
    }

    return bloques;
}

/*
Función que incializará el struct superbloque, del dispositivo virtual. El
superbloque es un bloque que contiene la información general del sistema de ficheros,
esta información nos ayudará para formar funciones de otros niveles.

Para inicializar el superbloque, deberemos leer su contenido, para posteriormente
darle valores a sus variables mediante las funciones anteriores, cuando acabemos de
inicializar el superbloque, lo escribiremos en su respectiva posición de nuevo.


ninodos: los inodos del dispositivo virtual
nbloques: bloques del dispositivo virtual
salida: EXITO si funcionó, FALLO en el caso de que bread o bwrite den fallo.

*/ 

int initSB(unsigned int nbloques, unsigned int ninodos)
{
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, RED"Error al leer el superbloque\n"RESET);
        return FALLO;
    }

    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    // Escribimos la estructura en el bloque posSB
    if (bwrite(posSB, &SB) == FALLO)
    {
        fprintf(stderr, RED"Error al escribir el superbloque en el bloque %d\n"RESET, posSB);
        return FALLO;
    }

    // printSB();

    return EXITO;
}






//función auxiliar initMB()
int elevar(int base,int exponente){
    int resultado=0;
    int dato;
    for (int i=1;i<exponente;i++){
        if(i==1){
            dato=base*base;
        }else{
            dato=dato*base;
        }
        resultado= dato;  
    }
    return resultado;
}

/*
Función que incializará el mapa de bits
Antes de hablar de la implementación deberemos conocer lo que es el MB, el mapa
de bits son un conjunto de bloques cuyos bits indican si el resto de bloques están ocupados.
Es decir, el primer bit del mapa de bits, indicará si el superbloque estará ocupado y así
sucesivamente(se indica con '1').

Por tanto, para inicializar estos bloques deberemos poner a '1' los bits que refercian
los bloques de los metadatos(porque estarán ocupados). Para conocer en qué bloques se encuentra
el MB deberemos leer el superbloque, lo que haremos es escribir 0 en los bloques del MB indicialmente
para poder cambiar los bits que son 1 posteriormente con escribir bit, también actualizaremos los bloques
libres cuando escribamos '1' en los ocupados. Finalmente guardaremos el superbloque escribiendolo
en el dispositivo de nuevo(muy importante porque cambiamos la cantidad de bloques libres).

salida: EXITO si funcionó o FALLO en el caso de errores

*/ 
int initMB()
{
    struct superbloque SB;
    int j = SB.posPrimerBloqueMB+1;
    // primero conseguiremos los datos necesarios para llamar a la
    // funcion tamAI y tamMB, haremos un bread.
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, "Error al leer el superbloque\n");
        return FALLO;
    }
    int metadatos = (tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos));
    
    // Creamos un buffer de memoria para almacenar el mapa de bits
    unsigned char bufferMB[BLOCKSIZE];
        if (memset(bufferMB, 0, BLOCKSIZE) == NULL)
    {
        return FALLO;
    }
    int bloquesocupados= (metadatos/8)/1024;
    int bytesa1 = metadatos / 8;

    int resto = metadatos % 8;
    int numBloqueAdicional = 0;
    int x = 0;
      for (int i = SB.posPrimerBloqueMB; i <=SB.posUltimoBloqueMB; i++)
    {
        if (bwrite(i, bufferMB) == FALLO)
        {
            return FALLO;
        }
    }
    // rellenamos el buffer con tantos unos como la variable bytesa1
    for (; x < (bytesa1-1); x++)
    {

        bufferMB[x] = 255;
    }
    // calculo de bloque restante
    if (resto != 0)
    {
        for (int i = 0; i < resto; i++)
        {
            numBloqueAdicional+=elevar(2,(8-(i+1))); 
        }
    }
    //printf("SOY NUMBLOQUEADICIONAL: %d \n",numBloqueAdicional);

    x=x+1;
    //printf("SOY X: %d \n",x);
    bufferMB[x] = numBloqueAdicional;
    // rellenamos el resto
    if (bwrite(SB.posPrimerBloqueMB,bufferMB)==FALLO){
        printf("ERROR AL ESCRIBIR EL PRIMER BLOQUE MB");
    }
    SB.cantBloquesLibres = SB.cantBloquesLibres - metadatos;
    //arreglo error rarungo
    for (int i=3128;i<=SB.posUltimoBloqueAI;i++){
        escribir_bit(i,1);
    }
    if (bwrite(posSB, &SB) == FALLO)
    {
        fprintf(stderr, "Error al escribir el superbloque en el bloque %d\n", posSB);
        return FALLO;
    }
   
   
    
    return EXITO;
}


/*
Función que incializará la lista de inodos libres de nuestro dispositivo virtual. El Array de Inodos
es una estructura que nos ayudará en funciones posteriores, pues un inodo contiene los metadatos
de un fichero o directorio y los punteros para obtener sus datos en la zona de datos.

Para recorrer el array de inodos, declararemos un array cuyo tamaño serán los inodos
que caben dentro de un bloque del dispositivo virtual, también es importante comenzar
a partir del inodo siguiente al primer inodo libre, pues estamos recorriendo la lista de inodos
LIBRES. Posteriormente, tendremos que leer cada bloque(almacenando su contenido en el array de inodos creado
por nosotors) del array de inodos(sabemos el bloque por el superbloque). Cuando obtengamos
los datos del bloque iesimo, iteraremos para cada uno de los inodos de ese bloque e iremos
indicando que está LIBRE y enlazandolo con el anterior inodo. Iteraremos por cada bloque
y inodos hasta llegar al ultimo inodo, el cual apuntará a un número muy grande.

Salida: Exito si funcionó, Fallo en el caso de que no.

*/  
int initAI()
{ 
    struct superbloque SB;
    // Leer el superbloque para obtener la localización del array de inodos
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, RED"Error al leer el superbloque\n"RESET);
        return FALLO;
    }

    // Inicializar la variable para el nº de inodo siguiente
    unsigned int contInodos = SB.posPrimerInodoLibre + 1;

    // Crear un buffer para los inodos
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Iterar para cada bloque del array de inodos
    for (unsigned int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        // Leer el bloque de inodos en el dispositivo virtual
        if (bread(i, inodos) == FALLO)
        {
            fprintf(stderr, RED"Error al leer el bloque de inodos %d\n"RESET, i);
            return FALLO;
        }

        // Iterar para cada inodo del bloque
        for (unsigned int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            // Indicar que el inodo está libre
            inodos[j].tipo = 'l'; // libre

            // Enlazar con el siguiente inodo si no hemos llegado al último inodo
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                // printf("%d ",contInodos); //DEBUG
                contInodos++;
            }
            else
            {
                // Hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                // Salir del bucle, el último bloque no tiene por qué estar completo
                break;
            }
        }

        // Escribir el bloque de inodos en el dispositivo virtual
        if (bwrite(i, inodos) == FALLO)
        {
            fprintf(stderr, RED"Error al escribir el bloque de inodos %d\n"RESET, i);
            return FALLO;
        }
    }

    return EXITO;
}







/* ----------------------------------------
                NIVEL 3
---------------------------------------- */

/*
Función que leerá un bit dentro del MB representado por el nbloque, deberemos
hacer una conversión del bloque físico a un bloque absoluto dentro del dispositivo.
Finalmente, haremos una serie de cálculos para encontrar el bit que debemos leer.

Esta función fue una gran fuente de error, porque pusimos la actualización de posbyte
antes de conseguir el nBloqueMB.

nbloque: bloque físico indicado

Salida: Valor del bit leido.

*/  
char leer_bit(unsigned int nbloque)
{
    struct superbloque sb;
    bread(posSB, &sb);
    unsigned char mascara = 128; // 10000000
    
    unsigned int posbyte = nbloque / 8;
    //ERROR QUE TARDÉ OCHO AÑOS EN ENCONTRAR,  EL AJUSTAMIENTO TIENE QUE SER DESPUÉS( DE POSBYTE)
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    posbyte = posbyte % BLOCKSIZE;
    unsigned int posbit = nbloque % 8;

   
    unsigned  int nbloqueAbs = nbloqueMB + sb.posPrimerBloqueMB;
   
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB,'\0',BLOCKSIZE);
    bread(nbloqueAbs,&bufferMB);
    #if DEBUGLVL3
        fprintf(stderr,GRAY"leer_bit()-> posbyte= %d, posbit=%d,nbloqueMB=%d,nbloqueabs: %d \n"RESET, posbyte, posbit, nbloqueMB,nbloqueAbs);
    #endif
    mascara >>= posbit;           // desplazamiento de bits a la derecha, los que indique posbit
    mascara &= bufferMB[posbyte]; // operador AND para bits
    mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha
                                  // para dejar el 0 o 1 en el extremo derecho y leerlo en decimal
   
    
    return mascara;
}
/*
Función que escribirá un bit dentro del MB representado por el nbloque, deberemos
hacer una conversión del bloque físico a un bloque absoluto dentro del dispositivo.
Finalmente, haremos una serie de cálculos para encontrar el bit que debemos modificar,
para finalmente salvar el bloque.

Esta función fue una gran fuente de error, porque pusimos la actualización de posbyte
antes de conseguir el nBloqueMB.

nbloque: bloque físico indicado
bit: valor a escribir
Salida: EXITO si funcionó o FALLO en el caso de que no.

*/  
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    struct superbloque sb;
    bread(posSB, &sb);
    char bufferMB[BLOCKSIZE];
    int posbyte = nbloque / 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int posbit = nbloque % 8;   

    int nbloqueAbs = nbloqueMB + sb.posPrimerBloqueMB;
    bread(nbloqueAbs,bufferMB);
    unsigned char mascara = 128; // 10000000
    mascara >>= posbit;          // desplazamiento de bits a la derecha
    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara; //  operador OR para bits
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // operadores AND y NOT para bits
    }
    bwrite(nbloqueAbs,bufferMB);
    #if DEBUGLVL3
        fprintf(stderr,GRAY"escribir_bit()-> posbyte= %d, posbit=%d,nbloqueMB=%d,nbloqueabs: %d \n"RESET, posbyte, posbit, nbloqueMB,nbloqueAbs);
    #endif
    return EXITO;
}
/*
Función que escribirá el contenido de un inodo en la posición del array
indicada por ninodo. Para conocer el bloque en el que se encuentra el inodo
deberemos usar ninodo,INODOSIZE y BLOCKSIZE, cuando tengamos ese bloque
usaremos el bloque donde comienza el array de inodos para tener el bloque exacto
donde se encuentra el inodo indicado, leeremos ese bloque y mediante una operacion
conoceremos donde se encuentra exactamente dentro del bloque y le cambiaremos el contenido.
Finalmente escribiremos el bloque resultante.

ninodo= posición del inodo dentro del AI
inodo= puntero con el contenido a cambiar del inodo original.

salida: EXITO si funciona, FALLO en el caso de errores.
*/ 
int escribir_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, RED"Error en la lectura de superbloque"RESET);
        return FALLO;
    }
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    //encontras bloque exacto
    int bloqueAi = (ninodo * INODOSIZE) / BLOCKSIZE;
    int nbloqueabs = bloqueAi + SB.posPrimerBloqueAI;
    //leer bloque para después encontrar inodo.
    if (bread(nbloqueabs, inodos) == FALLO)
    {
        fprintf(stderr, RED"Error en la lectura del bloque indicado"RESET);
        return FALLO;
    }
    //asignación
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = *inodo;
    //salvado del bloque
    if (bwrite(nbloqueabs, inodos) == FALLO)
    {
        fprintf(stderr, RED"Error al escribir de nuevo los inodos"RESET);
        return FALLO;
    }
    return EXITO;
}
/*
Función que leerá el contenido de un inodo dentro del AI y 
lo volcará dentro del inodo pasado por parámetro.Sinceramente,
hace exactamente lo mismo que la anterior, pero aquí unicamente
conseguiremos el contenido del inodo deseado.

ninodo= posición del inodo dentro del AI
inodo= puntero al que le introduciremos el inodo leido.

salida: EXITO si funciona, FALLO en el caso de errores.
*/ 
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // leemos superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, RED"Error en la lectura de superbloque"RESET);
        return FALLO;
    }
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    int bloqueAi = (ninodo * INODOSIZE) / BLOCKSIZE;
    //conocer bloque exacto
    int nbloqueabs = bloqueAi + SB.posPrimerBloqueAI;
    //leer bloque para conseguir inodo exacto posteriormente
    if (bread(nbloqueabs, inodos) == FALLO)
    {
        fprintf(stderr, RED"Error en la lectura del bloque indicado"RESET);
        return FALLO;
    }
    //almacenar el contenido dentro del puntero de parámetro
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];
    return EXITO;
}

/*
Funcion reservar_bloque en el que buscaremos reservar el primer bloque vacio, entonces deberemos 
buscar el primer bloque vacio verificando los bits del mapa de bits, al conseguir el bloque 
en especifico conseguiremos su posicion en bytes para más tarde tener el bit correspondiente,
con los valores conseguidos podremos calcular ese bloque como bloque físico, para así escribir
un 1 en el mapa de bits para ponerlo como bloque reservado.

salida: FALLO o el numero de bloque físico.
*/
int reservar_bloque() {
    struct superbloque SB;
    // Leemos el Super Bloque
    if (bread(posSB, &SB) == -1) {
        fprintf(stderr, RED"Error en ficheros_basico.c reservar_bloque() --> %d: %s\n"RESET, errno, strerror(errno));
        return -1;
    }
    // Si no hay bloques libres, acaba con -1
    if (SB.cantBloquesLibres == 0) {
        fprintf(stderr, RED"Error en ficheros_basico.c reservar_bloque() --> %d: %s\nImposible reservar bloque, no quedan libres!\n"RESET, errno, strerror(errno));
        return -1;
    }
  unsigned int posBloqueMB = SB.posPrimerBloqueMB;
  unsigned char bufferMB[BLOCKSIZE];      // Buffer de lectura MB
  memset(bufferMB,'\0',BLOCKSIZE);
  unsigned char bufferAUX[BLOCKSIZE];     // Buffer auxiliario (todos 1)
  memset(bufferAUX,255,BLOCKSIZE);
  int verificar = 1;
  int bit = 0;
  unsigned char mascara = 128;

  // Recorremos los bloques del MB hasta encontrar uno que esté a 0
  for(; posBloqueMB <= SB.posUltimoBloqueMB && verificar > 0 && bit == 0; posBloqueMB++){
    verificar = bread(posBloqueMB, bufferMB);
    bit = memcmp(bufferMB,bufferAUX, BLOCKSIZE);
  }
  // Una vez encontrado, vamos atràs de 1
  posBloqueMB--;

  if(verificar > 0){
    for(int posbyte = 0; posbyte < BLOCKSIZE; posbyte++){
      if(bufferMB[posbyte] < 255) {
        int posbit = 0;
        while (bufferMB[posbyte] & mascara) {
          posbit++;
          bufferMB[posbyte] <<= 1;  // Desplazamos bits a la izquierda
        }
        // Ahora posbit contiene el bit = 0
        int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB)* BLOCKSIZE + posbyte) * 8 + posbit;
        escribir_bit(nbloque, 1);                             // Marcamos el bloque como reservado
        SB.cantBloquesLibres = SB.cantBloquesLibres - 1;      // Actualizamos cantidad de bloques libres en el SB
        // Guardamos el SB
        if (bwrite(posSB, &SB) == -1) {
          fprintf(stderr, RED"Error en ficheros_basico.c reservar_bloque() --> %d: %s\n"RESET, errno, strerror(errno));
          return -1;
        }
        // Grabamos un buffer de 0s en la pos. nbloque
        unsigned char bufferVacio[BLOCKSIZE];      // Buffer de ceros
        memset(bufferVacio,'\0',BLOCKSIZE);
        if (bwrite(nbloque, &bufferVacio) == -1) {
          fprintf(stderr, RED"Error en ficheros_basico.c reservar_bloque() --> %d: %s\nImposible escribir buffer vacio"RESET, errno, strerror(errno));
          return -1;
        }
        return nbloque;
      }
    }
  }
  fprintf(stderr, RED"Error en ficheros_basico.c reservar_bloque() --> %d: %s\nNo es valido"RESET, errno, strerror(errno));
  return -1;
}

/*
Funcion liberar_bloque que únicamente pondremos el numero de bloque pasado por parametros como
bloque libre(por lo tanto se queda basura en la posicion de ese bloque en especifico)

nbloque=numero de bloque fisico que queremos liberar

salida: FALLO o el numero de bloque físico.
*/
int liberar_bloque(unsigned int nbloque)
{
    escribir_bit(nbloque, 0);
    struct superbloque sb;
    bread(posSB, &sb);
    sb.cantBloquesLibres++;
    bwrite(posSB, &sb);
    return nbloque;
}

/*
Funcion reservar_inodo, en la que reservaremos el primer inodo libre, así como
inicializando todos su componentes a partir de las parametros como tipo y permisos.

tipo=tipo de inodo fichero o directorio
permisos= permisos a asignar como escribir o leer

salida: FALLO o el numero de bloque físico.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    unsigned int posInodoReservado;
    
    // 1. Comprobar si hay inodos libres y si no hay inodos libres indicar error y salir.
    if (bread(posSB, &SB) == FALLO)
    { // Leer el superbloque
        fprintf(stderr, RED "Error al leer el superbloque\n" RESET);
        return FALLO;
    }

    if (SB.cantInodosLibres == 0)
    { // Comprobar si hay inodos libres
        fprintf(stderr, RED "No hay inodos libres disponibles\n" RESET);
        return FALLO;
    }
    posInodoReservado= SB.posPrimerInodoLibre;
    struct inodo inodo;
    if(leer_inodo(SB.posPrimerInodoLibre,&inodo)==FALLO){
        return FALLO;
    }
    
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);

    // 2. Actualizar la lista enlazada de inodos libres de tal manera que el superbloque apunte al siguiente de la lista.
    // Tendremos la precaución de guardar en una variable auxiliar posInodoReservado cual era el primer inodo libre, ya que éste es el que hemos de devolver.
    posInodoReservado = SB.posPrimerInodoLibre; // Guardar en una variable auxiliar posInodoReservado cual era el primer inodo libre

    for (int i = 0; i < 12; i++)
    {
        inodo.punterosDirectos[i] = 0;
    }
    inodo.punterosIndirectos[0] = 0;
    inodo.punterosIndirectos[1] = 0;
       inodo.punterosIndirectos[2]=0;
    escribir_inodo(posInodoReservado, &inodo);
    // 5. Decrementar la cantidad de inodos libres, y reescribir el superbloque.
    SB.cantInodosLibres--;
    SB.posPrimerInodoLibre = posInodoReservado + 1;
    bwrite(posSB, &SB);

    return posInodoReservado++; // 6. Devolver posInodoReservado.
}




/* ----------------------------------------
                NIVEL 4
---------------------------------------- */

/*
Función cuya funcionalidad es detectar el rango en el que se encuenta un inodo,
usando como ayuda unos parámetros declarados en ficheros_basico.h que indican
los rangos de los punteros directos, indirectos[0],indirectos[1] y indirectos[2],
con rangos nos referimos a bloques lógicos. Cuando encontremos el rango donde se
encuentra el inodo, deberemos actualizar "ptr" para que apunte donde lo hace el
puntero del inodo.

inodo: puntero a inodo, que contiene los datos para guardar en ptr
nblogico: bloque lógico en el que se encuentr el inodo
ptr: puntero en el que almacenaremos el puntero correspondiente al puntero
del inodo.

salida: 0,1,2,3 según el rango encontrado, FALLO si hubo un error.IMPORTANTE,
aquí devolvemos los bloques de punteros que habrá, por tanto para nosotros
el rango 0, será el primero, el rango 1 será el siguiente y así consecutivamente.
*/
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    {
        *ptr = 0;
        fprintf(stderr, RED"Bloque lógico fuera de rango"RESET);
        return FALLO;
    }
}

/*
Función que se complementa con nRangoBL, debido a que su funcionalidad principal
es encontrar un bloque físico correspondiente a un bloque lógico concreto dentro de un rango determinado. 
El funcionamiento de esta función se basa en restar los punteros anteriores y 
dividir en el caso de indirectos1 y indirectos2.

nblogico= indica el bloquelogico a buscar
nivel_punteros= indica los niveles de punteros existentes, hay a partir
de indirectos1, donde tenemos que acceder a bloques de punteros, para finalmente
poder acceder a bloques de datos.

salida: bloque físico encontrado o FALLO en el caso de error.
*/
int obtener_indice(unsigned int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }
    else if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS;
    }
    else if (nblogico < INDIRECTOS1)
    {

        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {

            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS) ;
        }
        else if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS ;
        }
    }
    return FALLO;
}

/*
Función que se sirve de  nRangoBL y obtenerIndice para realizar una función más compleja.
Lo que hará esta función es obtener el número de bloque físico correspondiente a un bloque lógico
determinado del inodo indicado. Nosotros hemos decidido implementar la versión iterativa
de esta función, tenemos un parámetro reservar en esta función porque en el futuro usará
mi_read_f(no reserva bloque) y mi_write_f(puede necestiar reservarlo).

Por tanto, si reservar = 0, indica que el bloque de datos existe, por tanto la función
devolverá su posición, si no existe dará error.

Si reservar=1, primero consultaremos, si no existe lo reservaremos y daremos su posición.

También es importante conocer si los bloques de punteros que apuntan a los bloques de datos que buscamos
existen, en el caso de que estemos en reservar=0 daremos erros y si estamos en reservar=1 tendremos
que reservarlos y enlazarlos.

Por tanto, resumiendo lo que haremos será: obtener los bloques de punteros intermedios que debería
haber mediante nRangoBL, además de tener el puntero del inodo. Entramos en un bucle
en el que comprobaremos por cada nivel de punteros, si cuelgan bloques de punteros,
mediante ptr==0, si ptr==0 también nos indica que el bloque no existe, por tanto
deberemos emitir el error correspondiente en el caso de lectura. Pero, si estamos
en el caso escritura, reservaremos el bloque y modificaremos los datos
correspondientes del inodo. Cuando ya tengamos el bloque reservado, deberemos comprobar
si el inodo debe apuntar directamente a el o  habrá un bloque de punteros anterior
que apuntará a ese bloque de punteros, podemos comprobar esto con (nivel_punteros==nRangoBL),
si esto se cumple unicamente deberemos enlazar el puntero al bloque de punteros que acabamos
de reservar, si no ocurre, deberemos guardar en el dispositivo el buffer de punteros
con el ptr(ha sido modificado cuando hemos reservado el bloque), que encontramos anteriormente, para
formar la estructura de los bloques de punteros.

Ahora, en el caso de que cuelgen bloques de punteros, leeremos el bloque de punteros del dispositivo
y lo guardaremos en un buffer y escribiremos en el dispositivo virtual(para formar la estructura). 
Posteriormente, en cada caso usaremos obtener indice, para encontrar el bloque físico indicado y desplazaremos
el puntero encontrado al siguiente nivel. Haremos todo esto para 
cada uno de los niveles de punteros que deberia haber dentro del rango, de tal manera que al finalizar el bucle deberiamos
encontrarnos en el bloque de datos correspondiente, aunque aquí, puede haber otro caso y es que
no exista el bloque de datos correspondiente, haremos lo mismo, si reservar es 0 emitiremos el error,
si reservar es 1 reservaremos el bloque y modificaremos el inodo, aunque también hay que tener en cuenta
si nos encontramos en el primer rango(no hay bloques de punteros, por tanto no habremos entrado al bucle).
En ese caso, ya tenemos el bloque logico(el de parámetro) y el puntero, obtenido mediante obtenernRangoBL. En el caso
de que no estemos en punteros directos, guardaremos el puntero en el buffer y lo escribiremos en el dispositivo.

inodo= inodo a buscar.
nblogico= bloque lógico en el que se encuentra el inodo.
reservar= si es 0 usaremos el modo lectura, si es 1 usaremos el modo escritura.
*/
int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar)
{
    unsigned int   ptr, ptr_ant;  
    int nRangoBL, nivel_punteros, indice;  
    unsigned int buffer[NPUNTEROS];

    ptr=0;
    ptr_ant=0;
    //obtención de los bloques de punteros intermedios
    nRangoBL=obtener_nRangoBL(inodo,nblogico,&ptr);

    nivel_punteros=nRangoBL;
    while (nivel_punteros > 0)
    {
        if (ptr == 0) //no cuelgan bloques de punteros
        {
            if (reservar == 0) //si reservar==0 no podremos reservarlos
            {
                return -1;
            }
            else
            {
                //reservaremos el bloque de punteros, modificaremos
                //el inodo y si el bloque cuelga directamente del inodo
                //haremos que apunte al bloque de punteros
                ptr = reservar_bloque();

                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);
                if (nivel_punteros == nRangoBL) //¡CUELGA DIRECTAMENTE!
                {
                    //inodo apuntará al bloque de punteros
                    inodo->punterosIndirectos[nRangoBL - 1] = ptr;
                    #if DEBUGLVL4
                        fprintf(stderr,GRAY"traducir_bloque_inodo()→ inodo.punterosIndirectos[%d] = %d \n"RESET,nRangoBL,ptr);
                    #endif
                }
                else
                {
                    //si el inodo no apunta directamente a este bloque, deberemos
                    //guardarlo en el dispositivo para formar la estructura.
                    buffer[indice] = ptr;
                    #if DEBUGLVL4
                        fprintf(stderr,GRAY"traducir_bloque_inodo()→punteros_nivel%d[%d] = %d \n"RESET,nivel_punteros+1,indice,ptr);
                    #endif
                    if(bwrite(ptr_ant, buffer)==FALLO){
                        return FALLO;
                    }
                    
                }
                //limpiar, por si acabamos guardando basura.
                memset(buffer, 0, BLOCKSIZE);
            }
        }
        else
        { // los bloques de punteros existen, por tanto
            //los guardaremos en el dispositivo virtual para
            //formar la estructura.
            if(bread(ptr, buffer)==FALLO){
                return FALLO;
            }
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;        // guardamos el puntero actual
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel
        
        nivel_punteros--;
        
    }
    if (ptr == 0)
    {   //BLOQUE DE DATOS NO EXISTE
        if (reservar == 0)
        { //MODO LECTURA
            return -1;
        }
        else
        {
            //modo escritura, reservamos el bloque y 
            //modificamos los datos del inodo
            ptr = reservar_bloque();
            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);

            if (nRangoBL == 0)
            {   //bloque de datos cuelga directamente del inodo
                inodo->punterosDirectos[nblogico] = ptr;
                #if DEBUGLVL4
                    fprintf(stderr,GRAY"[traducir_bloque_inodo()→ inodo.punterosDirectos[%d] = %d \n"RESET,nblogico,ptr);
                #endif
            }
            else
            {
                buffer[indice] = ptr; // asignamos la dirección del bloque de datos en el buffer
                #if DEBUGLVL4
                    fprintf(stderr,GRAY"traducir_bloque_inodo()→punteros_nivel%d[%d] = %d \n"RESET,nivel_punteros+1,indice,ptr);
                #endif
                if(bwrite(ptr_ant, buffer)==FALLO){
                    return FALLO;
                }
             
            }
        }
    }
    return ptr;
}


/* ****** FIN NIVEL 4 ****** */


/* ----------------------------------------
                  NIVEL 6
---------------------------------------- */

/*

Función cuya funcionalidad principal es liberar un inod con todos los bloques
de datos que estaba ocupando, a su vez, cuando liberamos un inodo deberemos
cambiar ciertos parámetros del mismo y superbloque. Para liberar los bloques
de un inodo usaremos una función del mismo nivel llamada liberar_bloques_inodo,
a esta función le pasaremos por parámetro un 0, ya que deberemos liberar todos los bloques
ocupados, desde el 0 hasta el ultimo bloque de ese inodo.

ninodo:posicion del inodo dentro del AI.

salida: inodo liberado, o fallo en el caso de errores
*/

int liberar_inodo(unsigned int ninodo) {
    //1. Leer el inodo
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    
    unsigned int numBloquesALiberar;
    // 2.Llamar a la función auxiliar liberar_bloques_inodo() para liberar todos los bloques del inodo
    numBloquesALiberar = liberar_bloques_inodo(0, &inodo);
    if (numBloquesALiberar == FALLO){
        return FALLO;
    }
    
    // 3.Restar la cantidad de bloques liberados de la cantidad de bloques ocupados del inodo
    inodo.numBloquesOcupados -= numBloquesALiberar;
    
    // 4.Marcar el inodo como tipo libre y tamEnBytesLog=0
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;
    
    // 5.Actualizar la lista enlazada de inodos libres
    struct superbloque sb;
    if (bread(posSB, &sb) == FALLO){
        return FALLO;
    }
    inodo.punterosDirectos[0] = sb.posPrimerInodoLibre;
    sb.posPrimerInodoLibre = ninodo;
    
    // 6.Incrementar la cantidad de inodos libres
    sb.cantInodosLibres++;
    
    // 7. Escribir el superbloque actualizado
    bwrite(posSB, &sb);

    // 8. Actualizar ctime
    time_t current_time = time(NULL);
    inodo.ctime = current_time;
    
    // 9. Escribir el inodo actualizado
    escribir_inodo(ninodo, &inodo);
    
    return ninodo; //10. Devolver el nº del inodo liberado
}

/*
    ***FUNCIÓN AUXILIAR RECURSIVA*** BASADA EN EL PSEUDOCÓDIGO DEL DOCUMENTO DE NIVEL 6 
    para liberar_bloques_inodo()
*/

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo) { //ok?
    unsigned int nivel_punteros = 0, nBL = primerBL, ultimoBL, ptr = 0;
    int nRangoBL = 0, liberados = 0, eof = 0;
    int total_breads = 0, total_bwrites = 0; //NUEVO

    if (inodo->tamEnBytesLog == 0) {return 0;} // el fichero está vacío

    // Obtenemos el último bloque lógico del inodo
    if (inodo->tamEnBytesLog % BLOCKSIZE == 0) {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    } else {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }
    #if DEBUGLVL6
        fprintf(stderr, GRAY"[liberar_bloques_inodo()→ primer BL: %u, último BL: %u]\n"RESET, primerBL, ultimoBL);
    #endif

    nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
    if (nRangoBL == 0) {
        liberados += liberar_directos(&nBL, ultimoBL, inodo, &eof);
    }

    while (!eof) {
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
        nivel_punteros = nRangoBL;
        //liberados += liberar_indirectos_recursivo(&nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros, &ptr, &eof);
        liberados += liberar_indirectos_recursivo(&nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros, &ptr, &eof, &total_breads, &total_bwrites); //NUEVO
    }
    #if DEBUGLVL6
        fprintf(stderr, GRAY"[liberar_bloques_inodo()→ total bloques liberados: %d, total_breads: %d, total_bwrites: %d]\n"RESET, liberados, total_breads, total_bwrites);
    #endif
    return liberados;
}

int liberar_directos(unsigned int *nBL, unsigned int ultimoBL, struct inodo *inodo, int *eof) { //ok?
    int liberados = 0;

    for (unsigned int d = *nBL; d < DIRECTOS && !(*eof); d++) { 
        if (inodo->punterosDirectos[*nBL] != 0) {
            liberar_bloque(inodo->punterosDirectos[*nBL]);
            #if DEBUGLVL6
                fprintf(stderr, GRAY"[liberar_bloques_inodo()→ liberado BF %u de datos para BL %u]\n"RESET, inodo->punterosDirectos[*nBL], *nBL);
            #endif
            inodo->punterosDirectos[*nBL] = 0;
            liberados++;
        }
        *nBL = *nBL+1;  
        if (*nBL > ultimoBL) {*eof = 1;} // Fin del archivo
    }

    return liberados;
}

//int liberar_indirectos_recursivo(unsigned int *nBL, unsigned int primerBL, unsigned int ultimoBL, struct inodo *inodo, int nRangoBL, unsigned int nivel_punteros, unsigned int *ptr, int *eof) { //ok?
int liberar_indirectos_recursivo(unsigned int *nBL, unsigned int primerBL, unsigned int ultimoBL, struct inodo *inodo, int nRangoBL, unsigned int nivel_punteros, unsigned int *ptr, int *eof, int *total_breads, int *total_bwrites){ //NUEVO
    int liberados = 0, indice_inicial;
    unsigned int bloquePunteros[NPUNTEROS], bloquePunteros_Aux[NPUNTEROS], bufferCeros[NPUNTEROS];
    memset(bufferCeros, 0, BLOCKSIZE);

    if (*ptr) { // si cuelga un bloque de punteros
        indice_inicial = obtener_indice(*nBL, nivel_punteros);
        if (indice_inicial == 0 || *nBL == primerBL) { // solo leemos bloque si no estaba cargado
            if (bread(*ptr, bloquePunteros) == -1) {return FALLO;}
            (*total_breads)++; //NUEVO    
            // Guardamos copia del bloque para ver si hay cambios
            memcpy(bloquePunteros_Aux, bloquePunteros, BLOCKSIZE);
        }

        // exploramos el bloque de punteros iterando el índice
        for (unsigned int i = indice_inicial; i < NPUNTEROS && !(*eof); i++) { 
            if (bloquePunteros[i] != 0) {
                if (nivel_punteros == 1) {
                    liberar_bloque(bloquePunteros[i]); // de datos
                    #if DEBUGLVL6
                        fprintf(stderr, GRAY"[liberar_bloques_inodo()→ liberado BF %u de datos para BL %u]\n"RESET, bloquePunteros[i], *nBL);
                    #endif
                    bloquePunteros[i] = 0;
                    liberados++;
                    *nBL = *nBL+1; 
                } else { // llamada recursiva para explorar el nivel siguiente de punteros hacia los datos
                    //liberados += liberar_indirectos_recursivo(nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros - 1, &bloquePunteros[i], eof);
                    liberados += liberar_indirectos_recursivo(nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros - 1, &bloquePunteros[i], eof,total_breads,total_bwrites); //NUEVO
                }
            } else { // bloquePunteros[i] == 0
                switch (nivel_punteros) { // Saltos al valer 0 un puntero según nivel
                    case 1: 
						*nBL = *nBL+1; 
                        break;
                    case 2: 
						*nBL += NPUNTEROS; 
                        break;
                    case 3: 
						*nBL += NPUNTEROS*NPUNTEROS; 
                        break;
                }
            }
            if (*nBL > ultimoBL) {*eof = 1;} // Comprobamos si hemos llegado al fin del archivo
        }

        // Si el bloque de punteros es distinto al original
        if (memcmp(bloquePunteros, bloquePunteros_Aux, BLOCKSIZE) != 0) {
            // si quedan punteros != 0 en el bloque lo salvamos
            if (memcmp(bloquePunteros, bufferCeros, BLOCKSIZE) != 0) {
                bwrite(*ptr, bloquePunteros);
                (*total_bwrites)++; //NUEVO
            } else {
                liberar_bloque(*ptr); // de punteros
                #if DEBUGLVL6
                    fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %u de punteros_nivel%d correspondiente al BL %u]\n", *ptr, nivel_punteros, *nBL);
                #endif
                *ptr = 0; // ponemos a 0 el puntero que apuntaba al bloque liberado
                liberados++;
            }
        }
    } else { // *ptr == 0
        // sólo entrará si es un puntero del inodo, resto de casos sólo se llama recursivamente con *ptr != 0
        switch (nRangoBL) { // Saltos al valer 0 un puntero del inodo según nivel
            case 1: 
				*nBL = INDIRECTOS0;
                break;
            case 2: 
				*nBL = INDIRECTOS1;
                break;
            case 3: 
				*nBL = INDIRECTOS2;
                break;
        }
    }
	
    return liberados;
}
