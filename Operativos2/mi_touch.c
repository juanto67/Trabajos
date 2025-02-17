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

  /* DIVISIÓN MI_TOUCH/MI_MKDIR */
  int longitud = strlen(ruta);
  if (ruta[longitud - 1] == '/') {
    fprintf(stderr, "mi_touch()-> No es un fichero\n");
    exit(EXIT_FAILURE);
  }
  /*   */
  
  if (bmount(nombre_dispositivo) == FALLO) {
    fprintf(stderr, "mi_touch()->Error al montar el dispositivo virtual\n");
    exit(EXIT_FAILURE);
  }

  if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
    fprintf(stderr, "mi_touch()-> Permisos no validos\n");
    exit(EXIT_FAILURE);
  }

  unsigned char permisos = atoi(argv[2]);
  if (mi_creat(ruta,permisos)==FALLO){
    fprintf(stderr, "mi_touch()->Error al usar mi_creat()\n");
    exit(EXIT_FAILURE);
  }

  if (bumount() == FALLO) {
    fprintf(stderr, "mi_touch()->Error al desmontar el dispositivo virtual\n");
    exit(EXIT_FAILURE);
  }
}