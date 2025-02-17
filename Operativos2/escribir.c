/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "ficheros.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    char *texto = argv[2]; //VERIFICAR PUNTERO
    int diferentes_inodos = atoi(argv[3]);

    // Abrir el dispositivo
    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, RED"escribir.c: Error al montar el dispositivo.\n"RESET);//perror("Error al montar el dispositivo");
        exit(EXIT_FAILURE);
    }

    // Reservar inodo
    int ninodo = reservar_inodo('f', 6);
    if (ninodo == FALLO) {
        fprintf(stderr, RED"escribir.c: Error al reservar el inodo.\n"RESET);//perror("Error al reservar inodo");
        bumount();
        exit(EXIT_FAILURE);
    }

    long int nbytes = strlen(texto);
    printf("longitud texto: %ld\n", nbytes);

    int offsetArr[5] = {9000, 209000, 30725000, 409605000, 480000000};
    // Bucle que escribe los 5 offsets
    for (int i = 0; i < 5; i++) {
        // Mostrar el número de inodo reservado
        fprintf(stderr,"\nNúmero de inodo reservado: %u\n", ninodo);
        printf("offset: %d\n", offsetArr[i]);

        // Escribir texto en el inodo
        int escritosReales = mi_write_f(ninodo, texto, offsetArr[i], nbytes);
        if (escritosReales == FALLO) {
            perror("Error al escribir en el inodo");
            bumount();
            exit(EXIT_FAILURE);
        }
        fprintf(stderr,"Bytes escritos: %d\n", escritosReales);

        // Mostrar tamaño en bytes lógicos del inodo y número de bloques ocupados
        struct STAT stat;
        if (mi_stat_f(ninodo, &stat) == FALLO) {
            perror("Error al obtener estadísticas del inodo");
            bumount();
            exit(EXIT_FAILURE);
        }
        printf("stat.tamEnBytesLog= %u\n", stat.tamEnBytesLog);
        printf("stat.numBloquesOcupados= %u\n\n", stat.numBloquesOcupados);
        
        // Si diferentes_inodos = 0 se reserva un solo inodo para todos los offsets.
        if (diferentes_inodos!=0) {
            ninodo = reservar_inodo('f', 6);
            if (ninodo == FALLO) {
                perror("Error al intenrar reservar un inodo");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, RED"escribir.c: Error al desmontar el dispositivo.\n"RESET);//perror("Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
