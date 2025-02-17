/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

int main(int argc, char **argv){
 if ((argc>4 || argc<3)) {
        fprintf(stderr, "Uso: %s <disco> </ruta>\n", argv[0]);
        fprintf(stderr, "Uso: %s -l <disco> </ruta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char flag;
    
    char * nombre_dispositivo;
    char * ruta;
    if(argc==4){
        //conseguir solo la L
    flag = *(argv[1]+1);
  
    
    
    nombre_dispositivo= argv[2];
    ruta=argv[3];
    }else{
        nombre_dispositivo = argv[1];
        ruta= argv[2];
        flag='n';
    }
    

    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, "mi_ls()->Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    
    char buffer[TAMBUFFER];
    int entradas=mi_dir(ruta,buffer,flag);
    if (entradas==FALLO){

    fprintf(stderr, "mi_ls()->Error al usar mi_dir()\n");
      exit(EXIT_FAILURE);
    }
    printf("Total: %d \n",entradas);
     if (argc==4 && entradas>0){
          printf("Tipo\tModo\tmTime\t\t    Tamaño\tNombre\n");
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    }
    
    
    printf("%s",buffer);
     if (bumount() == FALLO) {
        fprintf(stderr, "mi_ls()->Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    return EXITO;
}
