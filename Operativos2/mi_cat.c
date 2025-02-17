/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s  <disco> </ruta_fichero> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int tambuffer=1500;
    char *nombre_dispositivo = argv[1];
    char *ruta = argv[2];
    char buffer [tambuffer];
    int offset=0;
    int bytesleidos=0;
    memset(buffer, 0, sizeof(buffer)); //inicialkizar buffer a 0
    
   if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr,"mi_cat.c()->Error al montar el dispositivo \n");
        exit(EXIT_FAILURE);
    }

    int error=  mi_read(ruta,buffer,offset,sizeof(buffer));
    
    while (error>0){
        bytesleidos+=error;
        
        //printf("%s",buffer); //NIET
        write(1, buffer, error);  //CON ESTO FUNCIONA XD

        //igual habría que limpiar el buffer xd
        memset(buffer, 0, sizeof(buffer));
        offset+=error;
        //offset+=tambuffer; //? equivalentes
        
        error=  mi_read(ruta,buffer,offset,sizeof(buffer));
    }
  
    fprintf(stderr, " \n"); /*/////*/
    fprintf(stderr,"mi_cat.c()-> Hemos leido = %d bytes\n",bytesleidos); //esto habría que imprimirlo por stderr creo, que sino luego se mete al hacer ''exportaciones'' a ficheros y esas cosas
    
   
    //probablemente se podría añadir alguna gestión de errores por aquí, por si da -1 (o menos que -1, que implicaría error al buscar entrada)

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        perror("mi_escribir.c()->Error al desmontar el dispositivo");
        exit(EXIT_FAILURE);
    }
}
