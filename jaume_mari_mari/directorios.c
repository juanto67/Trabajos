/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
#include "directorios.h"

/* ----------------------------------------
                  NIVEL 7
---------------------------------------- */

/*
Metodo extraer_camino que a partir de los parametros la ruta(camino), en el que conseguiremos los valores correspondientes
a los parametros inicial, final y tipo. Para ello se confirmará que el camino es correcto para luego conseguir el tipo, como
más tarde la ruta inicial y la siguiente(final)

Este metodo al utilizar punteros estaremos buscando como resultado el cambio de los punteros inicial, final y tipo.
return:EXITO O FALLO
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    // El camino debe empezar por /, si no es incorrecto...
    if (camino[0] != '/') {  return ERROR_CAMINO_INCORRECTO; }
    char *parte2 = strchr(camino + 1, '/'); //siguiente '/'  

    if (parte2) //Si hay otra '/'
    {
        strncpy(inicial, camino + 1, strlen(camino) - strlen(parte2) - 1);
        strcpy(final, parte2);

        if (final[0] == '/') { //si acaba en '/' es directorio y le ponemos tipo 'd'
            *tipo = 'd';
        }
    }
    else // Si no hay más "/"
    { 
        strcpy(inicial, camino + 1); 
        strcpy(final, ""); 
        *tipo = 'f';
    }

    return EXITO;
}


/*
Metodo recursivo buscar_entrada donde buscaremos entrada a entrada hasta conseguir llegar a la ruta en cuestion pasada por parametros, por tanto
deberemos utilizar el metodo extraer_camino con el objetivo de ir pasando de la ruta actual a la siguiente. Tendremos el p_inodo_dir que sera el ninodo
del inodo padre en el que estamos buscando la entrada, p_inodo será al acabar la primera iteración el siguiente ninodo padre. Después tenemos
el p_entreda que sera el numero de entras que hemos recorrido hasta llegar a la que buscabamos.Finalmente el valor reservar que al ser 1 representa
la creacion del fichero o directorio correspondiente y permisos serás los permisos correspondientes a este fichero o directorio.

return:EXITO O FALLO

Lo importante de esta función es conseguir los numeros de inodo correspondiente a la ruta que buscamos
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, 
  unsigned int *p_entrada, char reservar, unsigned char permisos){
    struct superbloque SB;
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    if (bread(posSB, &SB)==FALLO){ 
        fprintf(stderr,RED"ERROR LECTURA SUPERBLOQUE"RESET);
    }

    if (!strcmp(camino_parcial, "/")){  //camino_parcial es “/”
        *p_inodo=SB.posInodoRaiz;  //nuestra raiz siempre estará asociada al inodo 0
        *p_entrada=0;
        return EXITO;
    }
 
    //memsets a 0 por si acaso
    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));

    if(extraer_camino(camino_parcial, inicial, final, &tipo)<0){
        return ERROR_CAMINO_INCORRECTO;
    }

    #if DEBUGLVL7
        fprintf(stderr,GRAY"[buscar_entrada() → inicial: %s, final: %s, reservar: %d]\n"RESET, inicial, final, reservar);
    #endif
      
   //buscamos la entrada cuyo nombre se encuentra en inicial
   leer_inodo(*p_inodo_dir, &inodo_dir);
   if ((inodo_dir.permisos & 4)!=4){
    return ERROR_PERMISO_LECTURA;
   } 

    memset(entrada.nombre, 0, sizeof(entrada.nombre)); // inicializar el buffer de lectura con 0s
    
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada); 
    num_entrada_inodo = 0;
    if (cant_entradas_inodo > 0)
    {
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == FALLO) { //leer entrada
            return ERROR_PERMISO_LECTURA;
        }

        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0) {
            num_entrada_inodo++;
            memset(entrada.nombre, 0, sizeof(entrada.nombre)); //previamente volver a inicializar el buffer de lectura con 0s
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == FALLO) { // leer siguiente entrada
                return ERROR_PERMISO_LECTURA;
            }
        }
    }

if ((strcmp(entrada.nombre, inicial)) &&(num_entrada_inodo == cant_entradas_inodo)) { 
  //la entrada no existe
       switch(reservar){
           case 0:  //modo consulta. Como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
                break; 
           case 1:  //modo escritura 
                //Creamos la entrada en el directorio referenciado por *p_inodo_dir
               //si es fichero no permitir escritura
               if (inodo_dir.tipo == 'f') {
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
               }
              //si es directorio comprobar que tiene permiso de escritura
               if ((inodo_dir.permisos&2)!=2){
                    return ERROR_PERMISO_ESCRITURA;
               }else{
                   strcpy(entrada.nombre, inicial); //copiar *inicial en el nombre de la entrada
                   if (tipo == 'd'){  
                    if (!strcmp(final, "/")){ 
                            entrada.ninodo=reservar_inodo(tipo,permisos);//reservar un inodo como directorio y asignarlo a la entrada (por lógica, tipo=='d')
                            #if DEBUGLVL7
                                printf(GRAY"[buscar_entrada() → reservado inodo %d tipo %c con permisos %d para %s]\n"RESET, entrada.ninodo, tipo, permisos, entrada.nombre);
                            #endif
                       }else{ //no es el final de la ruta
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                       }
                   }else{ //es un fichero
                        entrada.ninodo=reservar_inodo(tipo,permisos);//reservar un inodo como fichero y asignarlo a la entrada (por lógica tipo=='f')
                        #if DEBUGLVL7
                            printf(GRAY"[buscar_entrada() → reservado inodo %d tipo %c con permisos %d para %s]\n"RESET, entrada.ninodo, tipo, permisos, entrada.nombre);
                        #endif
                   }

                #if DEBUGLVL7
                    printf(GRAY"[buscar_entrada() → creada entrada: %s, %d] \n"RESET, inicial, entrada.ninodo);
                #endif

                   //escribir la entrada en el directorio padre
                  if(mi_write_f(*p_inodo_dir,&entrada,num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada))<0){              
                    if (entrada.ninodo != -1){  //entrada.inodo != -1
                        liberar_inodo(entrada.ninodo);
                        #if DEBUGLVL7
                            printf(GRAY"[buscar_entrada() → liberado inodo %i, reservado a %s\n"RESET, num_entrada_inodo, inicial);
                        #endif
                    }
                    return FALLO; //-1
                  }
               }   
        }
    }


   if (strcmp(final, "") == 0 || strcmp(final, "/")==0) { 
       if ((num_entrada_inodo < cant_entradas_inodo) && (reservar==1)) {
            //modo escritura y la entrada ya existe
	       
            return ERROR_ENTRADA_YA_EXISTENTE;
       }
       // cortamos la recursividad
       *p_inodo = entrada.ninodo;
       *p_entrada=num_entrada_inodo;
       
       return EXITO; //0
   }else{
        *p_inodo_dir = entrada.ninodo; 
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
   }
   return EXITO; //0
}

/*
Metodo mostrar_error_buscar_entrada que dependiendo del valor pasado por parametro hara degub de un error en concreto.
*/
void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -2:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -3:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -6:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -7:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -8:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}


/* ----------------------------------------
                  NIVEL 8
---------------------------------------- */

/*
Metodo mi_creat con el cual buscaremos crear una ruta a partir del metodo buscar_entrada con el valor a reservar a 1, por lo tanto 
necesitaremos el camino correpondiente como los permisos.

return:EXITO O FALLO
*/
int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();
    unsigned int pinododir = 0;
    unsigned int pinodo = 0;
    unsigned int pentrada = 0;
    
    if (!((permisos <= 7) && (permisos >= 0)))
    {
         mi_signalSem();
	    fprintf(stderr, RED"Error: Mal formato de permisos(0-7).\n"RESET);
        return FALLO;
    }
   
    int error = buscar_entrada(camino, &pinododir, &pinodo, &pentrada, 1, permisos);
    if (error < 0)
    {
         mi_signalSem();
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
     mi_signalSem();
    return EXITO;
}
/*
    Función cuya funcionalidad principal consiste en mostrar los
    datos de un directorio mediante un buffer. Para implementar esto, internamente
    deberemos leer las entradas de un inodo, por lo tanto, deberemos leer cada entrada y
    posteriormente el inodo asignado a dicha entrada para obtener la información a visualizar,
    nosotros utilizamos un buffer auxiliar donde introducimos toda la información a visualizar para
    después introducirla en el buffer pasado por parámetro.Hemos introducido 3 mejoras
    en esta función.

    1.- Usar el buffer de entradas para no hacer tantas operaciones de E/S. A la hora
    de implementar esta mejora, es importante el cálculo del tamaño del array y un error que teniamos,
    era que usabamos el array pero igualmente metiamos el read_f hasta sizeof entradas dentro del for,
    lo que sería lo mismo que no hacerla, por tanto también es importante leer lo equivalente a todo el buffer
    antes del bucle.

    2.- Opción de mi_ls -l, esta mejora consiste en ampliar la información visualizada,
    a la hora de implementarla, tuvimos problemas porque pensabamos hacerlo con strcat, pero
    al ver lo que se hace con el tiempo, entendimos que es mejor utilizar sprintf.

    3.- Opción de ls simple o largo, simplemente usamos el parámetro flag, el cual hemos implementado
    que sea unicamente 'l' en el caso de que sea largo, hacemos una comparación en el caso de que lo sea
    y si lo es, también tenemos en cuenta el print de la cabecera.

    Salida= Si funciona, el buffer tendrá el contenido de la información a visualizar
    y devolverá el numero de entradas leidas.

*/
int mi_dir(const char *camino, char *buffer ,char flag)
{
    struct inodo inodo;
    struct inodo inodoAuxiliar;
    unsigned int p_inododir=0;
    unsigned int p_inodo=0;
    unsigned int p_entrada=0;
    //IMPORTANTE declaras esto como array, si no dará SEGMENTION FAULT en los
    //sprintf.
    char tmp[50];
    char tipoI[2];
    char tamaño[10];
    
    char bufferAuxiliarDatos [TAMBUFFER];
    int entrada= buscar_entrada(camino,&p_inododir,&p_inodo,&p_entrada,0,4);
    //lectura inodo para conseguir el numero de entradas.
    if(leer_inodo(p_inodo,&inodo)==FALLO){
        fprintf(stderr,"mi_dir()-> error leer inodo");
        return FALLO; 
    }

    if (entrada<0){
        mostrar_error_buscar_entrada(entrada);
        return FALLO;
    }
    
    if (inodo.tipo!='d'){
        mostrar_error_buscar_entrada(-8);
        return FALLO; 
    }
    if ((inodo.permisos & 4) != 4) {
        mostrar_error_buscar_entrada(-3);
        return FALLO;
    }
    int cant_entradas = inodo.tamEnBytesLog/ sizeof( struct entrada);
    struct entrada bufferAyuda [cant_entradas];
    //obtención de todas las entradas
    if ((mi_read_f(p_inodo,&bufferAyuda,0,sizeof(bufferAyuda))<0)){
          
            return FALLO;
        }
     //bucle de formación del buffer  
    for(int i=0;i<cant_entradas;i++){
        memset(bufferAuxiliarDatos,'\0',TAMBUFFER);
        
        if (leer_inodo(bufferAyuda[i].ninodo,&inodoAuxiliar) < 0) {
            fprintf(stderr, "mi_dir()  --> No se ha podido leer el inodo asociado a la entrada \n");
            return FALLO;
        }
        
         //TIPO 
        if (flag=='l'){
        sprintf(tamaño,"%d",inodoAuxiliar.tamEnBytesLog);
        sprintf(tipoI, "%c", inodoAuxiliar.tipo);
        strcat(bufferAuxiliarDatos,tipoI);
        strcat(bufferAuxiliarDatos,"\t");
        
      
      
            
        // PERMISOS
           if (inodoAuxiliar.permisos & 4) strcat(bufferAuxiliarDatos, "r"); else strcat(bufferAuxiliarDatos, "-");
       if (inodoAuxiliar.permisos & 2) strcat(bufferAuxiliarDatos, "w"); else strcat(bufferAuxiliarDatos, "-");
       if (inodoAuxiliar.permisos & 1) strcat(bufferAuxiliarDatos, "x"); else strcat(bufferAuxiliarDatos, "-");

        
     // MTIME
    
       
      struct tm *tm; 
       tm = localtime(&inodoAuxiliar.mtime);
       sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);
       strcat(bufferAuxiliarDatos,"\t");
       strcat(bufferAuxiliarDatos, tmp);
        strcat(bufferAuxiliarDatos,"\t");
       
       //TAMAÑO
        strcat(bufferAuxiliarDatos,tamaño);

        strcat(bufferAuxiliarDatos,"\t");
        }
        //NOMBRE
        strcat(bufferAuxiliarDatos, bufferAyuda[i].nombre);
        if(flag=='l'){
        strcat(bufferAuxiliarDatos,"\n");
        }else{
          strcat(bufferAuxiliarDatos,"\t");  
        }
         strcat(buffer,bufferAuxiliarDatos);
       


    }

    return cant_entradas;
}
/*
Metodo mi_chmod,metodo en el que se buscara la ruta pasada por parametros para cambiar los permisos de ese numero de inodo 

return:FALLO O EXITO

*/
int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int pinododir = 0;
    unsigned int pinodo = 0;
    unsigned int pentrada = 0;
    

    int buscarentrada_resultado = buscar_entrada(camino, &pinododir, &pinodo, &pentrada, 0, permisos);

    if (buscarentrada_resultado < 0)
    {
	mostrar_error_buscar_entrada(buscarentrada_resultado);
        return buscarentrada_resultado;
    }

    return mi_chmod_f(pinodo, permisos);
}

/*
Metodo mi_stat que como anteriormente se busca el camino asignado, para más tarde cambiar los valores 
del struct inodo correspondiente al numero de inodo conseguido

return:FALLO O numero de inodo cambiado

*/
int mi_stat(const char *camino, struct STAT *p_stat)
{
    unsigned int pinododir = 0;
    unsigned int pinodo = 0;
    unsigned int pentrada = 0;
    
    
    int buscarentrada_resultado = buscar_entrada(camino, &pinododir, &pinodo, &pentrada, 0,4);
    if(buscarentrada_resultado<0)
    {
   	mostrar_error_buscar_entrada(buscarentrada_resultado);
        return FALLO;
    }
     if(mi_stat_f(pinodo, p_stat)==FALLO){
        fprintf(stderr, "mi_stat() -> Error ejecución de mi_stat_f()");
        return FALLO;
     }

     return pinodo;
}
/*
Metodo mi_write en el que buscaremos una ruta, para concluir con la escritura en ese directorio o fichero del contenido del buff pasado por parametros,
con un offset y una cantidad de bytes correspondientes.

return:FALLO o numero de bytes escritos

*/

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    // respecto a devolver nº de bytes escritos, supongo que basta con lo que devuelve mi_write_f (?) se presupone que debería estar bien, creo yo
    // MEJORA POSIBLE
   
    unsigned int p_inodo=0;
    unsigned int p_inodo_dir=0;
    unsigned int p_entrada=0;
    int nada=buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,4);
    if(nada<0){
 	mostrar_error_buscar_entrada(nada);
        return FALLO;
    }
  
    return mi_write_f(p_inodo, buf, offset, nbytes);

}

/*
Metodo mi_read en el que buscaremos una ruta, con el objetido de posteriormente 
leer el contenido en ese numero de inodo para asignarlo al buff pasado por parametros, sabiendo el offset y numero de bytes correspondientes.

return:FALLO O numero de bytes leidos

*/

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
    // MEJORA POSIBLE
    unsigned int p_inodo=0;
    unsigned int p_inodo_dir=0;
    unsigned int p_entrada=0;
    int nada1=buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,4);
    if(nada1<0){
   	mostrar_error_buscar_entrada(nada1);
        return FALLO;
    }
    int leidos=mi_read_f(p_inodo, buf, offset, nbytes);
    if(leidos==FALLO){
           fprintf(stderr,"mi_read()-> error mi_read_f()");
        return FALLO;
    }
    return leidos;
}



/* ----------------------------------------
                  NIVEL 10
---------------------------------------- */

/*
Metodo mi_link, metodo que conseguira hacer un link entre dos rutas distintas, para ello se tendra que crear la ruta del camino2 con
reservar de buscar entrada a 1, para luego asignar el contenido de la entrada a un struct entrada temporal asignando a esa entrada temporal
el numero inodo de la primera ruta. Después volveremos a escribir la entrada en el numero de inodo del padre de la ruta 2, asi haciendo que
en ese inodo padre tendra el numero de inodo siguiente a la ruta 1.

return:FALLO O EXITO
*/
int mi_link(const char *camino1, const char *camino2){
    mi_waitSem();

    unsigned int p_inodo1_dir=0;
    unsigned int p_inodo1=0;
    unsigned int p_entrada1=0;
    struct entrada entrada;
    struct inodo inodo;
    unsigned int p_inodo2_dir=0;
    unsigned int p_inodo2=0;
    unsigned int p_entrada2=0;
    int nada2=buscar_entrada(camino1,&p_inodo1_dir,&p_inodo1,&p_entrada1,0,4);
    if(nada2<0){
        mi_signalSem();
        mostrar_error_buscar_entrada(nada2);
        return FALLO;
    }
    int nada3=buscar_entrada(camino2,&p_inodo2_dir,&p_inodo2,&p_entrada2,1,6);
    if(nada3<0){
        mi_signalSem();
        mostrar_error_buscar_entrada(nada3);
        return FALLO;
    }
    //leer entrada
    if(mi_read_f(p_inodo2_dir,&entrada,p_entrada2*sizeof(struct entrada),sizeof(struct entrada))<0){
            mi_signalSem();
         fprintf(stderr,"mi_link()-> Error leer entrada");
          return FALLO;
    }
    entrada.ninodo= p_inodo1;
    //escribir entrada
    if(mi_write_f(p_inodo2_dir,&entrada,p_entrada2*sizeof(struct entrada),sizeof(struct entrada))<0){
            mi_signalSem();
         fprintf(stderr,"mi_link()-> Error escribir entrada");
          return FALLO;
    }
    if(liberar_inodo(p_inodo2)<0){
            mi_signalSem();
        fprintf(stderr,"mi_link()-> Error al liberar inodo");
          return FALLO;
    }

    if(leer_inodo(p_inodo1,&inodo)<0){
            mi_signalSem();
        fprintf(stderr,"mi_link()-> Error al leer inodo");
         return FALLO;
    }
    inodo.nlinks= inodo.nlinks+1;
    inodo.ctime = time(NULL);
    escribir_inodo(p_inodo1,&inodo);
        mi_signalSem();
    return EXITO;
 }

/*
Metodo mi_unlink ahora queremos hacer unlink de un camino con otro, por tanto deberemos deshacer el link,para ello deberemos llegar
a la ruta que buscamos(asegurandonos que que si es un directorio tiene que estar vacio), para confirmar si es la última entrada si es así
solo la eliminamos con el metodo mi_truncar_f sino intercambiaremos la ultima entrada con la que queremos eliminar, para finalmente eliminar la que queremos
como la última.

return:FALLO O EXITO
*/
int mi_unlink(const char *camino){
    mi_waitSem();
    unsigned int p_inodo=0;
    unsigned int p_inodo_dir=0;
    unsigned int p_entrada=0;
	int nada4=buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4);
    // comprobar que la entrada camino exista y obtener su nº de entrada (p_entrada), mediante la función buscar_entrada().
    if(nada4< 0){
        mi_signalSem();
	    mostrar_error_buscar_entrada(nada4);
        return FALLO;
    }

    struct inodo inodo;
    struct inodo inodo_dir;
    struct entrada entrada;
    if (leer_inodo(p_inodo, &inodo) == FALLO){ 
        mi_signalSem();
        return FALLO; } // Leer el inodo del archivo a borrar
    if((inodo.tipo == 'd') && (inodo.tamEnBytesLog > 0)){
        mi_signalSem(); // Verificar si el inodo es un directorio no vacío
        return FALLO; //Si se trata de un directorio  y no está vacío (inodo.tamEnBytesLog > 0) entonces no se puede borrar y salimos de la función.
    }
    if (leer_inodo(p_inodo_dir, &inodo_dir) == FALLO){ 
        mi_signalSem();
        return FALLO; } // Leer el inodo del directorio que contiene la entrada que queremos eliminar (p_inodo_dir)
    int numero_entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada); // Calcular el número de entradas en el directorio

	if(p_entrada == numero_entradas - 1){ 
        //Si la entrada a eliminar es la última (p_entrada ==nº entradas - 1), basta con truncar el inodo a su tamaño menos el tamaño de una entrada, mediante la función mi_truncar_f().
		mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada));
        
    }else{
		if (mi_read_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (numero_entradas - 1), sizeof(struct entrada)) == FALLO) { return FALLO; }
        if (mi_write_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (p_entrada), sizeof(struct entrada)) == FALLO) { return FALLO; }
		mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada));
    }
	

    
    inodo.nlinks--; // Decrementar el número de enlaces del inodo   
    if (inodo.nlinks == 0){ // Si no quedan enlaces, liberar el inodo
        if (liberar_inodo(p_inodo) < 0){ 
            mi_signalSem();
            
        return FALLO; }
    } else { inodo.ctime = time(NULL); } // Actualizar ctime

    // Escribir el inodo modificado
    if (escribir_inodo(p_inodo, &inodo) < 0){ 
        mi_signalSem();
        return FALLO; }

    return EXITO;
}

