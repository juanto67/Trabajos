/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

int main(int argc, char **argv){
 if (argc != 4) {
        fprintf(stderr, "Uso: %s <disco> </ruta_fichero_original> </ruta_enlace>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char *nombre_dispositivo = argv[1];
    char * rutaficheroor= argv[2];
    char * final;
    char * rutaenlace=argv[3];
    final=strchr(rutaficheroor, 47);
    if (strcmp(final,"/")==0){
        fprintf(stderr,"mi_link.c-> ruta fichero original no es un fichero");
    }
    if (bmount(nombre_dispositivo) == FALLO) {
        perror("mi_link.c->Error al montar el dispositivo");
        exit(EXIT_FAILURE);
    }
    if(mi_link(rutaficheroor,rutaenlace)==FALLO){
        fprintf(stderr,"mi_link.c->Error en la ejecución de mi_link");
    }


     // Desmontar el dispositivo
    if (bumount() == FALLO) {
        perror("mi_link.c->Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }
    
}
