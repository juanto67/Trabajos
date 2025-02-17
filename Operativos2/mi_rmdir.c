/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"
int main(int argc, char **argv){
 if (argc != 3) {
        fprintf(stderr, "Uso: %s <disco> </ruta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *nombre_dispositivo = argv[1];
    char * ruta= argv[2];

    /* DIVISIÓN MI_RM/MI_RMDIR */
    int longitud = strlen(ruta);
    if (ruta[longitud - 1] != '/') {
        fprintf(stderr, "mi_rmdir()-> No es un directorio\n");
        exit(EXIT_FAILURE);
    }
    /*   */

    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, "mi_chmod()->Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    if(mi_unlink(ruta)==FALLO){
        fprintf(stderr, "mi_unlink()->Error al unlink\n");
        exit(EXIT_FAILURE);
    }
     if (bumount() == FALLO) {
        fprintf(stderr, "mi_chmod()->Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    return EXITO;
}
