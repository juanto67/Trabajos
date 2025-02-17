/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
//#include "bloques.h"
#include "ficheros_basico.h"

//Ahora hay que incorporar en mi_mkfs.c un include de ficheros_basico.h (en vez de bloques.h) y las llamadas a las funciones initSB(), initMB(), initAI() para mejorar el formateo del sistema de ficheros del nivel 1. 

/* ------ NIVEL 1+2+3 ------ */
int main(int argc, char **argv){
    /* ------ NIVEL 1 ------ */
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <nbloques>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    unsigned int nbloques = atoi(argv[2]);
    unsigned int ninodos = nbloques / 4;

    // Montar el dispositivo virtual
    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }

    //FORMATEAR TODO A 0
    // Crear un buffer para escribir bloques inicializados a 0
    unsigned char *buffer = (unsigned char *)malloc(BLOCKSIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Error al asignar memoria para el buffer\n");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, BLOCKSIZE);

    // Escribir bloques inicializados a 0 en el dispositivo virtual
    for (unsigned int i = 0; i < nbloques; i++) {
        if (bwrite(i, buffer) == FALLO) {
            fprintf(stderr, "Error al escribir en el dispositivo virtual\n");
            exit(EXIT_FAILURE);
        }
    }

    // Liberar el buffer
    free(buffer);

    /* ------ FIN NIVEL 1 ------ */


    /* ------ NIVEL 2 ------ */        
    if (initSB(nbloques, ninodos) == FALLO) {
        fprintf(stderr, "Error al inicializar el SuperBloque\n");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar el Mapa de Bits
    if (initMB() == FALLO) {
        fprintf(stderr, "Error al inicializar el Mapa de Bits\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar el Array de Inodos
    if (initAI() == FALLO) {
        fprintf(stderr, "Error al inicializar el Array de Inodos\n");
        exit(EXIT_FAILURE);
    }

    /* ------ FIN NIVEL 2 ------ */

    /* ------ NIVEL 3 ------ */

    // Crear el directorio raíz [[[NIVEL 3]]
    if (reservar_inodo('d', 7) == FALLO) {
        fprintf(stderr, "Error al crear el directorio raíz\n");
        exit(EXIT_FAILURE);
    }

    /* ------ FIN NIVEL 3 ------ */

    /* ------ NIVEL 1 ------ */

    // Desmontar el dispositivo virtual
    if (bumount() == FALLO) {
        fprintf(stderr, "Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }

    printf("Dispositivo virtual formateado correctamente\n");
    return EXITO;
    /* ------ FIN NIVEL 1 ------ */
}
/* ------ FIN NIVEL 1+2+3 ------ */