/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include "ficheros.h"
int main(int argc, char *argv[]) {
    // Verificamos que se proporcionen suficientes argumentos
    if (argc != 4) {
        fprintf(stderr,"INCORRECTA SINTAXIS(Uso: %s <nombre_dispositivo> <ninodo> <nbytes>)\n", argv[0]); return FALLO;
    }

    // Obtenemos los valores de los argumentos y los almacenamos en variables
    char *nombre_dispositivo = argv[1];
    int ninodo = atoi(argv[2]); // Convertimos el argumento a entero usando atoi
    int nbytes = atoi(argv[3]); // Convertimos el argumento a entero usando atoi

    if(bmount(nombre_dispositivo)==FALLO){
        fprintf(stderr,"NO HA SIDO POSIBLE MONTAR EL DISPOSITIVO VIRTUAL\n"); return FALLO;
    }

    if(nbytes== 0){
       //liberar_inodo(ninodo);  //POR ESTO FALLABA XDDDDDD estaba duplicado
       if (liberar_inodo(ninodo) == FALLO) {
        fprintf(stderr, "ERROR LIBERAR INODO %i\n", ninodo); return FALLO;
        }
    }else{
        mi_truncar_f(ninodo,nbytes);
    } 

    struct STAT stat;
    if(mi_stat_f(ninodo,&stat)==FALLO){
        printf("algo ha fallado en mi_stat_f al truncra");
        return FALLO;
    }

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    //Información del inodo escrito
    printf("\nDATOS INODO %d:\n", ninodo);
    printf("tipo=%c\n", stat.tipo);
    printf("permisos=%d\n", stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nLinks= %d\n", stat.nlinks);
    printf("tamEnBytesLog= %d\n", stat.tamEnBytesLog);
    printf("numBloquesOcupados= %d\n", stat.numBloquesOcupados);

    if(bumount()==FALLO){
        printf("NO HA SIDO POSIBLE DESMONTAR EL DISPOSITIVO VIRTUAL");
        return FALLO;
    }

    return EXITO;
}
