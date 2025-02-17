/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include <stdlib.h>
#include "directorios.h"
/*
 * VERSION escibir para NIVEL 9
*/

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <disco> </ruta_fichero> <texto> <offset>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    char *ruta = argv[2];
    char *texto  = argv[3];
    unsigned int offset= atoi(argv[4]);
    
    // Abrir el dispositivo
    if (bmount(nombre_dispositivo) == FALLO) {
        perror("mi_escribir.c()->Error al montar el dispositivo");
        exit(EXIT_FAILURE);
    }
  
   int bytesescritos=mi_write(ruta, texto, offset, strlen(texto));
    if (bytesescritos == FALLO) {
        perror("mi_escribir.c()-> bytes escritos erroneo");
        bumount();
        exit(EXIT_FAILURE);
    }
    printf("mi_escribir.c()-> bytes escritos: %d",bytesescritos);
  
   
    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        perror("mi_escribir.c()->Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
