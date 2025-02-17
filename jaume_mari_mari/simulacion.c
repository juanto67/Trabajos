/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "simulacion.h"

int acabados = 0; // NOTA 8: Tendremos una variable global, acabados, inicializada a 0 para llevar la cuenta del nº de procesos finalizados, y que el enterrador irá incrementado.

void reaper()
{ // si no la añadimos al .h tiene que ir antes :)
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
    }
}

int main(int argc, char **argv)
{

    signal(SIGCHLD, reaper);
    if (argc != 2)
    {
        return FALLO;
    }
    const char *ruta = argv[1];
    if (bmount(ruta) == FALLO)
    {
        return FALLO;
    }
    unsigned int pinododir = 0;
    unsigned int pinodo = 0;
    unsigned int pentrada = 0;
    struct tm *tm;
    char camino[22];

    char *tmp;
    int nescritura = 1;

    time_t fecha;
    char cad[17];
    int procesos = 1;
    pid_t pid;
    char *piddato;

    fecha = time(NULL);
    tm = localtime(&fecha);
    sprintf(cad, "%d%02d%02d%02d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    sprintf(camino, "/simul_%s/", cad);
    int nada5 = buscar_entrada(camino, &pinododir, &pinodo, &pentrada, 1, 7);
    if (nada5 < 0)
    {
        mostrar_error_buscar_entrada(nada5);
        return FALLO;
    }

    for (int procesos = 1; procesos <= NUMPROCESOS; procesos++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            if (bmount(ruta) == FALLO)
            {
                return FALLO;
            }

            char caminoaux1[35];
            char caminoaux2[46];
            char dato[12];
            // Crear el directorio del proceso hijo añadiendo el PID al nombre.
            unsigned int pinododir1 = 0;
            unsigned int pinodo1 = 0;
            unsigned int pentrada1 = 0;
            memset(caminoaux1, '/0', strlen(caminoaux1));
            memset(caminoaux2, '/0', strlen(caminoaux2));
            //
            strcat(caminoaux1, camino);

            sprintf(dato, "proceso_%d/", getpid());
            strcat(caminoaux1, dato);

            int nada6 = buscar_entrada(caminoaux1, &pinododir1, &pinodo1, &pentrada1, 1, 7);
            if (nada6 < 0)
            {
                mostrar_error_buscar_entrada(nada6);
                return FALLO;
            }

            strcat(caminoaux2, caminoaux1);
            // Crear el fichero prueba.dat dentro del directorio anterior.
            unsigned int pinododir2 = 0;
            unsigned int pinodo2 = 0;
            unsigned int pentrada2 = 0;

            strcat(caminoaux2, "prueba.dat");

            int nada7 = buscar_entrada(caminoaux2, &pinododir2, &pinodo2, &pentrada2, 1, 7);
            if (nada7 < 0)
            {
                mostrar_error_buscar_entrada(nada7);
                return FALLO;
            }
            srand(time(NULL) + getpid());
            for (int nescritura = 1; nescritura <= NUMESCRITURAS; nescritura++)
            {
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = nescritura;
                registro.nRegistro = rand() % REGMAX;
                if (mi_write(caminoaux2, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO)) == FALLO)
                {
                    fprintf(stderr, "simulacion.c-> error en mi_write() \n");
                    return FALLO;
                }
               // printf(RESET "simulacion.c-> escritura %d en %s \n", nescritura, caminoaux2);
                usleep(50);
            }
            printf(GREEN "PROCESO %d COMPLETADAS LAS %d en %s \n", procesos, NUMESCRITURAS, caminoaux2);
            if (bumount() == FALLO)
            {
                return FALLO;
            }
            exit(0);
        }
        usleep(150);
    }
    for (; acabados < NUMPROCESOS;)
    {
        pause();
    }
    if (bumount() == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}
