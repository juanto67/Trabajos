/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
          fprintf(stderr, "Uso: %s <disco> </ruta>\n", argv[0]);
            exit(EXIT_FAILURE);
    }
    char *nombre_dispositivo = argv[1];
    struct STAT p_stat;
    if ((bmount(nombre_dispositivo)) == FALLO)
    {

        fprintf(stderr, "mi_stat.c -> Error al montar el dispositivo virtual\n");

        return FALLO;
    }
    const char *camino = argv[2];
    
    int ninodo = mi_stat(camino, &p_stat);
    if (ninodo < 0)
    {

        fprintf(stderr, "mi_stat.c-> ninodo negativo, error en mi_stat\n");

    }
    else
    {
        printf("Nº de inodo: %d \n", ninodo);
        printf("Fichero: %s\n", nombre_dispositivo);
        printf("tamEnBytesLog: %u\n", p_stat.tamEnBytesLog);
        printf("Tipo: %c\n", p_stat.tipo);
        printf("Permisos: %d\n", p_stat.permisos);
        printf("atime: %s", ctime(&p_stat.atime));
        printf("mtime: %s", ctime(&p_stat.mtime));
        printf("ctime: %s", ctime(&p_stat.ctime));
        printf("nlinks: %d \n", p_stat.nlinks);
        printf("numBloquesOcupados: %u\n", p_stat.numBloquesOcupados);
    }
    if ((bumount()) ==FALLO)
    {

        fprintf(stderr, "mi_stat->Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);

    }
    return EXITO;
}
