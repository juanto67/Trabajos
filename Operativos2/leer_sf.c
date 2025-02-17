/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include "ficheros_basico.h"

int main(int argc, char **argv) {
    /* ------ NIVEL 2 ------ */
    // Comprobar número de argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Montar el dispositivo virtual
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    
    //int reservado1= reservar_inodo('f',6);  //quitado para LVL5
  

    // Leer el superbloque
    struct superbloque sb;
    bread(posSB, &sb);

    // Mostrar información del superbloque
    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %u\n", sb.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %u\n", sb.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %u\n", sb.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %u\n", sb.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %u\n", sb.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %u\n", sb.posUltimoBloqueDatos);
    printf("posInodoRaiz = %u\n", sb.posInodoRaiz);
    printf("posPrimerInodoLibre = %u\n", sb.posPrimerInodoLibre);
    printf("cantBloquesLibres = %u\n", sb.cantBloquesLibres);
    printf("cantInodosLibres = %u\n", sb.cantInodosLibres);
    printf("totBloques = %u\n", sb.totBloques);
    printf("totInodos = %u\n", sb.totInodos);
    printf("\nsizeof struct superbloque: %ld\n", sizeof(struct superbloque));
    printf("sizeof struct inodo: %ld\n", sizeof(struct inodo));
    
    #if DEBUGLVL3
        // Leer la lista enlazada de inodos libres
        printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
        for (unsigned int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) { // Iterar para cada bloque del array de inodos
            // Crear un buffer para los inodos
            struct inodo inodos[BLOCKSIZE / INODOSIZE];
            break;
            // Leer el bloque de inodos en el dispositivo virtual
            if (bread(i, inodos) == FALLO) {
                fprintf(stderr, RED"Error al leer el bloque de inodos %d\n"RESET, i);
                exit(EXIT_FAILURE);
            }

            // Iterar para cada inodo del bloque
            for (unsigned int j = 0; j < BLOCKSIZE / INODOSIZE; j++) {
                // Imprimir el valor de punterosDirectos[0] si el inodo está libre
                if (inodos[j].tipo == 'l') {
                    printf("%d ", inodos[j].punterosDirectos[0]);
                }
                // Manejar el caso en el que se llega al final de la lista de inodos libres
                if (inodos[j].punterosDirectos[0] == UINT_MAX) {
                    printf("-1\n");
                    return 1;
                }
            }
        }
        /* ****** FIN NIVEL 2 ****** */



        /* ------ NIVEL 3 ------ */
        printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
        printf("posSB: %d → leer_bit(%d) = %d\n", posSB, posSB, leer_bit(posSB));
        printf("SB.posPrimerBloqueMB: %d → leer_bit(%d) = %d\n", sb.posPrimerBloqueMB, sb.posPrimerBloqueMB, leer_bit(sb.posPrimerBloqueMB));
        printf("SB.posUltimoBloqueMB: %d → leer_bit(%d) = %d\n", sb.posUltimoBloqueMB, sb.posUltimoBloqueMB, leer_bit(sb.posUltimoBloqueMB));
        printf("SB.posPrimerBloqueAI: %d → leer_bit(%d) = %d\n", sb.posPrimerBloqueAI, sb.posPrimerBloqueAI, leer_bit(sb.posPrimerBloqueAI));
        printf("SB.posUltimoBloqueAI: %d → leer_bit(%d) = %d\n", sb.posUltimoBloqueAI, sb.posUltimoBloqueAI, leer_bit(sb.posUltimoBloqueAI));
        printf("SB.posPrimerBloqueDatos: %d → leer_bit(%d) = %d\n", sb.posPrimerBloqueDatos, sb.posPrimerBloqueDatos, leer_bit(sb.posPrimerBloqueDatos));
        printf("SB.posUltimoBloqueDatos: %d → leer_bit(%d) = %d\n", sb.posUltimoBloqueDatos, sb.posUltimoBloqueDatos, leer_bit(sb.posUltimoBloqueDatos));

        printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS \n");
        int reservado= reservar_bloque();
        printf("SE HA RESERVADO EL BLOQUE %d \n",reservado);
        
        bread(posSB,&sb);
        printf("BLOQUES LIBRES %d: \n",sb.cantBloquesLibres);
        liberar_bloque(reservado);
        bread(posSB,&sb);
        printf("DESPUÉS DE LIBERAR: %d \n",sb.cantBloquesLibres);
        struct inodo inodo;
        leer_inodo(0,&inodo);
        ///////--------------DATOS DIRECTORIO RAIZ----------------
        printf("DATOS DIRECTORIO RAIZ \n");
        printf("TIPO %c \n",inodo.tipo);
        printf("PERMISOS:%d \n",inodo.permisos);
        
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];
        
        ts = localtime(&inodo.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
        printf("ID: %d ATIME: %s MTIME: %s CTIME: %s\n",0,atime,mtime,ctime);

        printf("nlink:%d \n",inodo.nlinks);
        printf("tamEnBytesLog: %d \n",inodo.tamEnBytesLog);
        printf("numBloquesOcupados: %d \n",inodo.numBloquesOcupados);

        /* ****** FIN NIVEL 3 ****** */
    #endif
       
    /* ------ NIVEL 4 ------ */ //COMENTAR EN EL NIVEL 5
 /*      leer_inodo(reservado1,&inodo);
    printf("------------------CASO 8------------------ \n");
    traducir_bloque_inodo(&inodo,8,1);
    printf("\n");
    printf("------------------CASO 204------------------ \n");
    traducir_bloque_inodo(&inodo,204,1);
    printf("\n");
    printf("------------------CASO 30000------------------ \n");
    traducir_bloque_inodo(&inodo,30004,1);
    printf("\n");
    printf("------------------CASO 400000------------------ \n");
    traducir_bloque_inodo(&inodo,400004 ,1);
    printf("\n");
    printf("------------------CASO 468750------------------ \n");
    traducir_bloque_inodo(&inodo,468750 ,1);
    printf("\n");
    printf("------------------DATOS INODO RESERVADO----------------\n");
 
    struct tm *ts1;
    char atime1[80];
    char mtime1[80];
    char ctime1[80];
    ts1 = localtime(&inodo.atime);
    strftime(atime1, sizeof(atime1), "%a %Y-%m-%d %H:%M:%S", ts1);
    ts1 = localtime(&inodo.mtime);
    strftime(mtime1, sizeof(mtime1), "%a %Y-%m-%d %H:%M:%S", ts1);
    ts1 = localtime(&inodo.ctime);
    strftime(ctime1, sizeof(ctime1), "%a %Y-%m-%d %H:%M:%S", ts1);
    printf("ATIME: %s MTIME: %s CTIME: %s\n",atime1,mtime1,ctime1);
    printf("TIPO: %c \n",inodo.tipo);
    printf("PERMISOS: %d \n",inodo.permisos);
    printf("nlink:%d \n",inodo.nlinks);
    printf("tamEnBytesLog: %d \n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %d \n",inodo.numBloquesOcupados);
    printf("posPrimerInodoLibre = %u\n", sb.posPrimerInodoLibre);
*/ 
    /* ****** FIN NIVEL 4 ****** */

    /* ------ NIVEL 2 ------ */

    // Desmontar el dispositivo virtual
    if (bumount() == FALLO) {
        fprintf(stderr, "Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    

    return EXIT_SUCCESS;
    /* ****** FIN NIVEL 2 ****** */
}