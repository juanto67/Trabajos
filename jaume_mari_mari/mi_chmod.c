/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

int main(int argc, char **argv){
 if (argc != 4) {
        fprintf(stderr, "Uso: %s <disco> <permisos> </ruta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *nombre_dispositivo = argv[1];
    char *ruta= argv[3];
    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, "mi_chmod()->Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    
    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
        fprintf(stderr, "mi_chmod()-> Permisos no validos\n");
        exit(EXIT_FAILURE);
    }

    unsigned char permisos = (unsigned char) atoi(argv[2]);
    int michmod_resultado = mi_chmod(ruta,permisos);
    if (michmod_resultado<=FALLO){
        mostrar_error_buscar_entrada(michmod_resultado);
        fprintf(stderr, "mi_chmod()->Error al usar mi_chmod()\n");
        exit(EXIT_FAILURE);
    }
    
     if (bumount() == FALLO) {
        fprintf(stderr, "mi_chmod()->Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }

    return EXITO;
}
