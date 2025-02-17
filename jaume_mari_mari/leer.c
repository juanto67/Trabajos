/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "ficheros.h"

int main(int argc, char *argv[]) {
	int tambuffer = 1500; // Tamaño del buffer de lectura
    if (argc !=3) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <ninodo> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    int ninodo = atoi(argv[2]);

    // Montar
    if (bmount(nombre_dispositivo) == FALLO) {
        perror("Error al montar el dispositivo");
        exit(EXIT_FAILURE);
    }

    // Obtener información del inodo
    struct STAT stat;
    char buffer_texto[tambuffer]; // Buffer para almacenar el texto leído
    int offset = 0; // Offset de inicio de lectura
    int leidos; // Bytes leídos en cada llamada a mi_read_f()    
    int leidos_total;
    memset(buffer_texto, 0, tambuffer); // Limpiar el buffer de lectura con 0s

    leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer); // Leer el contenido del inodo bloque a bloque
    while (leidos > 0) {
		leidos_total = leidos_total+leidos;
        write(1, buffer_texto, leidos); // Mostrar el contenido leído por pantalla
        offset += tambuffer; // Actualizar el offset para la próxima lectura
        memset(buffer_texto, 0, tambuffer); // Limpiar el buffer de lectura con 0s
        leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer); // Leer el siguiente bloque del inodo
    }

    // Mostrar el número total de bytes leídos y el tamaño en bytes lógicos del inodo
    fprintf(stderr, "Bytes leídos: %d\n", leidos_total);

	if (mi_stat_f(ninodo, &stat) == FALLO) {
            perror("Error al obtener estadísticas del inodo");
            bumount();
            exit(EXIT_FAILURE);
	}
	
    fprintf(stderr, "Tamaño en bytes lógicos del inodo: %u\n", stat.tamEnBytesLog);

    // Desmontar
    if (bumount() == FALLO) {
        perror("Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }

    return EXITO;
}
