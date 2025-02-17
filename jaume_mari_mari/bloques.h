/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
// bloques.h

#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()


#define BLOCKSIZE 1024 // bytes


#define EXITO 0 //para gestión errores
#define FALLO -1 //para gestión errores

/* --- DEBUGS VARIOS NIVELES --- */
//#define DEBUGLVL1 0 //nada a debuggear, sólo imprime errores si suceden
//#define DEBUGLVL2 0 //creo que no hay nada a debuggear
#define DEBUGLVL3 0 //Nivel 3: ficheros_basico.c esencialmente escribir_bit() y leer_bit() + leer_sf RECORRIDO LISTA ENLAZADA DE INODOS LIBRES
#define DEBUGLVL4 0 //Nivel 4: traducir_bloque_inodo() 
#define DEBUGLVL5 0 //Nivel 5: ficheros.c {mi_write_f(), mi_read_f(), mi_chmod_f(), mi_stat_f()} y escribir.c, leer.c, permitir.c
#define DEBUGLVL6 0 //Nivel 6: liberar_bloques_inodo()
#define DEBUGLVL7 0 //BUSCAR_ENTRADA 
//#define DEBUGLVL8 1 //ningún debug añadido //Nivel 8: directorios.c {mi_creat(), mi_dir(), mi_chmod(), mi_stat()}, y mi_mkdir.c, [mi_touch.c], mi_ls.c [-l], mi_chmod.c, mi_stat.c
//#define DEBUGLVL9 0 //ningún debug añadido //Nivel 9: directorios.c {mi_write(), mi_read()}, y mi_escribir.c, mi_cat.c
//#define DEBUGLVL10 0 //ningún debug añadido //Nivel 10: directorios.c {mi_link(), mi_unlink()}, mi_link.c, mi_rm.c, [mi_rmdir], [mi_rm_r] y scripts

/* COLORES */
#define BLACK   "\x1B[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1B[37m"
#define ORANGE  "\x1B[38;2;255;128;0m"
#define ROSE    "\x1B[38;2;255;151;203m"
#define LBLUE   "\x1B[38;2;53;149;240m"
#define LGREEN  "\x1B[38;2;17;245;120m"
#define GRAY    "\x1B[38;2;176;174;174m"
#define RESET   "\x1b[0m"


#define NEGRITA "\x1b[1m"


int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);
void mi_signalSem();
void mi_waitSem();