/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "verificacion.h"

int main(int argc, char **argv){
    char informe[100];
 if (argc != 3) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <directorio_simulacion>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *nombre_dispositivo = argv[1];
    char * ruta= argv[2];
    int offset=0;
    int offset3=0;
    char * piddato;
    int offset2=0;
    int validadas=0;
    char * caminoprueba;
     struct entrada * entrada;
     struct INFORMACION info;
     char * escritura;
      char * strpid;
      int cant_registros_buffer_escrituras = 256; 
 struct REGISTRO buffer_escrituras [cant_registros_buffer_escrituras];
 memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
    if (bmount(nombre_dispositivo) == FALLO) {
        fprintf(stderr, "verificacion()->Error al montar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    struct STAT dato;
    if(mi_stat(ruta,&dato)==FALLO){
         fprintf(stderr, "verificacion()->Error al usar mi_stat\n");
        exit(EXIT_FAILURE);
    }
    int nentradas= dato.tamEnBytesLog/sizeof(struct entrada);
    if (nentradas!=NUMPROCESOS){
         fprintf(stderr, "verificacion()-> ntentradas es diferente a NUMPROCESOS\n");
        exit(EXIT_FAILURE);
    }
    strcat(informe,ruta);
    strcat(informe,"informe.txt");
   
    if(mi_creat(informe,7)==FALLO){
         fprintf(stderr, "verificacion()-> error al intentar crear informe.txt\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i=0;i<nentradas;i++){
      //aquí da segmention fault porqué????
         if(mi_read(ruta,entrada,0,sizeof(entrada))==FALLO){
          fprintf(stderr, "verificacion()-> error lectura de las entradas\n");
        exit(EXIT_FAILURE);
    }
    offset= offset+sizeof(entrada);
    //creamos el camino
    strcat(caminoprueba,ruta);
    strcat(caminoprueba,entrada->nombre);
    strcat(caminoprueba,"prueba.dat");
    strcpy(piddato, entrada->nombre);
    piddato = strchr(piddato, '_'); //cogemos el PID del PROCESO
    int pid = atoi(piddato);
    strpid=("PID %d \n",pid);
    info.pid = pid;
     while (mi_read(caminoprueba, buffer_escrituras, offset2, sizeof(buffer_escrituras)) > 0) {

        for (int i=0;i<cant_registros_buffer_escrituras;i++){
              if (buffer_escrituras[i].pid == pid) {
               if (validadas==0){
                info.PrimeraEscritura = buffer_escrituras[i];
                info.UltimaEscritura = buffer_escrituras[i];
                info.MenorPosicion = buffer_escrituras[i];
                info.MayorPosicion = buffer_escrituras[i];

                validadas++;
               }else{
                 if (info.PrimeraEscritura.nEscritura > buffer_escrituras[i].nEscritura)
            {
              info.PrimeraEscritura = buffer_escrituras[i];
            }
            if (info.UltimaEscritura.nEscritura < buffer_escrituras[i].nEscritura)
            {
              info.UltimaEscritura = buffer_escrituras[i];
            }
            if (info.MenorPosicion.nRegistro > buffer_escrituras[i].nRegistro)
            {
              info.MenorPosicion = buffer_escrituras[i];
            }
            if (info.MayorPosicion.nRegistro < buffer_escrituras[i].nRegistro)
            {
              info.MayorPosicion = buffer_escrituras[i];
            }
            validadas++;
          }
          info.nEscrituras = validadas; //obtener escrituras de la última posición
               }
        {

        }
        int offset2=offset2+sizeof(buffer_escrituras);
        
         memset(buffer_escrituras, 0, sizeof(buffer_escrituras));

     }
     memset(escritura,"/0",sizeof(escritura));
   
     strcat(escritura,strpid);
   
     strcat(escritura,("Numero de escrituras: %d\n", validadas));

     strcat(escritura,("Primera Escritura:\t%d\t%d\t%d\t%s \n",info.PrimeraEscritura.nEscritura,info.PrimeraEscritura.nRegistro,info.PrimeraEscritura.pid,asctime(localtime(&info.PrimeraEscritura.fecha))));
     strcat(escritura,("Ultima Escritura:\t%d\t%d\t%d\t%s \n",info.UltimaEscritura.nEscritura,info.UltimaEscritura.nRegistro,info.UltimaEscritura.pid,asctime(localtime(&info.UltimaEscritura.fecha))));
     strcat(escritura,("Menor Posición:\t%d\t%d\t%d\t%s \n",info.MenorPosicion.nEscritura,info.MenorPosicion.nRegistro,info.MenorPosicion.pid,asctime(localtime(&info.MenorPosicion.fecha))));
     strcat(escritura,("Mayor Posición:\t%d\t%d\t%d\t%s \n",info.MayorPosicion.nEscritura,info.MayorPosicion.nRegistro,info.MayorPosicion.pid,asctime(localtime(&info.MayorPosicion.fecha))));
     printf("%s",escritura);
     if(mi_write(informe,&info,offset3,sizeof(info))==FALLO){
          fprintf(stderr, "verificacion->Error al escribir info\n");
        exit(EXIT_FAILURE);
     }
     offset3+=sizeof(info);
    }

     if (bumount() == FALLO) {
        fprintf(stderr, "verificacion->Error al desmontar el dispositivo virtual\n");
        exit(EXIT_FAILURE);
    }
    return EXITO;
}
}
