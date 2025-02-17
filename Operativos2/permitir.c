/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "ficheros.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <ninodo> <permisos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    unsigned int ninodo = atoi(argv[2]);
    unsigned char permisos = atoi(argv[3]);

    // Validación de permisos
    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Error: Los permisos deben ser un número entre 0 y 7.\n");
        exit(EXIT_FAILURE);
    }

    // Montar dispositivo
    if (bmount(nombre_dispositivo) == FALLO) {
        perror("Error al montar el dispositivo");
        exit(EXIT_FAILURE);
    }

    // Probar a cambiar permisos del inodo
    if (mi_chmod_f(ninodo, permisos) == FALLO) {
        perror("Error al cambiar permisos del inodo");
        bumount();
        exit(EXIT_FAILURE);
    }/*else{
        printf("permisos ((probablemente)) cambiados");
    }*/

    // Desmontar dispositivo
    if (bumount() == FALLO) {
        perror("Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }

    return EXITO;
}
