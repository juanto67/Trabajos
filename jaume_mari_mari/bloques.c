/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */

/* NIVEL 11 semaforos */
#include "semaforo_mutex_posix.h"
 static int descriptor = 0;

 static sem_t *mutex;
 static unsigned int inside_sc = 0;
/*

Funciones propias del semáforo mutex, usadas con la finalidad
de que si cambiamos el semáforo unicamente deberiamos cambiar
semaforo_mutex_posix. Por otro lado, se usa inside_sc, para 
evitar que se produzcan wait dos o más veces.

*/

   void mi_waitSem() {
   if (!inside_sc) { // inside_sc==0, no se ha hecho ya un wait
       waitSem(mutex);
   }
   inside_sc++;
}


void mi_signalSem() {
   inside_sc--;
   if (!inside_sc) {
       signalSem(mutex);
   }
}
   


/* ----------------------------------------
       NIVEL 1 (+ semaforos lvl 11)
---------------------------------------- */
   
/*

Función para montar el disco, dado que se trata de un fichero, usaremos la función
open, con el argumento pasado por parámetro(que indicará el nombre), además el uso de open
nos devolverá el descriptor del fichero, lo que nos servirá para desmontarlo. A partir del nivel
11, deberemos inicializar el semaforo en esta función.

camino: nombre del dispositivo.

salida: el descriptor si funciona, fallo en el caso de error.

*/
 int bmount(const char *camino) {
     if (descriptor > 0) {
       close(descriptor);
   }
   descriptor = open(camino, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   if (descriptor == -1) {
      fprintf(stderr, RED "Error al abrir el dispositivo virtual\n" RESET);
      return FALLO;
   }
    if (!mutex) { // el semáforo es único en el sistema y sólo se ha de inicializar 1 vez (padre)
       mutex = initSem(); 
       if (mutex == SEM_FAILED) {
           return FALLO;
       }
   }

   return descriptor;
 }

/*

Función para desmontar el disco, dado que se trata de un fichero, usaremos la función
close, con el descriptor(su le damos valor al montar el disco). A partir del Nivel 11,debemos
tener en cuenta eliminar el semáforo, ya que tiene que ser único.

salida: exito si funciona, fallo en el caso de error.
*/
 int bumount(){
  descriptor = close(descriptor);  //close() devuelve 0 en caso de éxito

   if (close(descriptor) == -1) {
      fprintf(stderr, RED "Error al desmontar el dispositivo virtual\n" RESET);
      return FALLO;
   }
   deleteSem(); 
   return EXITO;
 }

/*

Función para escribir un bloque físico en el dispositivo virtual, la posición de dicho bloque
está indicada por nbloque, además escribiremos el contenido del puntero apuntado por buf. En esta
función es importante calcular el offset exacto donde se encuentra el bloque a escribir y el uso de lseek
para poner el puntero del fichero en el punto correcto. Tras esto, usaremos la función write para escribir
el contenido de buf y comprobaremos si no se escribió un bloque entero.

nbloque: bloque físico en el que escribir.

buf: contiene el dato a escribir.

salida: los bytes escritos si funciona, fallo en el caso de error.
*/
 int bwrite(unsigned int nbloque, const void *buf){
  
   off_t offset = nbloque * BLOCKSIZE;

  
   if (lseek(descriptor, offset, SEEK_SET) == FALLO) {
      fprintf(stderr, RED "Error al posicionarse en el dispositivo virtual\n" RESET);
      return FALLO;
    }


   size_t bytes_escritos = write(descriptor, buf, BLOCKSIZE);
   if (bytes_escritos == -1) {
      fprintf(stderr, RED "Error al escribir en el dispositivo virtual\n" RESET);
      return FALLO;
    }

  
   if (bytes_escritos != BLOCKSIZE) {
      fprintf(stderr, RED "No se pudo escribir el bloque completo en el dispositivo virtual\n" RESET);
      return FALLO;
   }

   return bytes_escritos;
 }

/*
Función para leer un bloque físico en el dispositivo virtual, la posición de dicho bloque
está indicada por nbloque, meteremos el contenido del bloque dentro de buf. Tras esto, usaremos la función write para escribir
el contenido de buf y comprobaremos si no se escribió un bloque entero. Esta función
es parecida al bread, ya que es importante posicionar el puntero del fichero para leer
el bloque correcto y usaremos la función read para leer.

nbloque: bloque físico en el que leer.

buf: meteremos el contenido de lo leido.

salida: bytes leidos en el caso de que funciona, FALLO si hubo errores.

*/ 
int bread(unsigned int nbloque, void *buf){

   off_t offset = nbloque * BLOCKSIZE;
  
   if (lseek(descriptor, offset, SEEK_SET) == -1) {
      fprintf(stderr, RED "Error al posicionarse en el dispositivo virtual\n" RESET);
      return FALLO;
   }

   // Leemos el contenido del dispositivo virtual y lo almacenamos en el buffer
   size_t bytes_leidos = read(descriptor, buf, BLOCKSIZE);
   if (bytes_leidos == -1) {
      fprintf(stderr, RED "Error al leer del dispositivo virtual\n" RESET);
      return FALLO;
   }

   // Verificamos si se leyó el bloque completo
   if (bytes_leidos != BLOCKSIZE) {
      fprintf(stderr, RED "No se pudo leer el bloque completo del dispositivo virtual\n" RESET);
      return FALLO;
   }

   return bytes_leidos;
 }
