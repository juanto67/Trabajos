/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
 #include "ficheros.h"

/* ----------------------------------------
                  NIVEL 5
---------------------------------------- */

/*
Metodo mi_write_f el cual a partir de un numero de inodo, un buffer con contenido, un offset de inico y el numero
de bytes que queremos escribir, este escribirá el contenido del buffer original en un fichero a partir del bloque lógico

Metodo que necesitará tres casos claves qe son cuando solo es un bloque a escribir,cuando se opera con bloques intermedios
y cuando se llega al último bloque a escribir, todo para tener el control de las escrituras con el bwrite.

return:nbytes escritos

Estos irán aumentando conforme se vaya escribiendo
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    mi_waitSem();
    struct inodo inodo;
    int primerBL, ultimoBL, desp1, desp2;
    int bytesEscritos = 0;
    
    // Leer el inodo
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
        fprintf(stderr, RED"Error al leer el inodo\n"RESET);
        return FALLO;
    }

    // Comprobar permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        mi_signalSem();
        fprintf(stderr, RED"No hay permisos de escritura\n"RESET);
        return FALLO;
    }

    // Calcular primer y último bloque lógico
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    // Calcular desplazamientos dentro de bloques
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;
	
	int nbfisico;
	char buf_bloque[BLOCKSIZE];

	// Obtener el número de bloque físico correspondiente a primerBL
	nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
	if (nbfisico == FALLO) {
        mi_signalSem();
		fprintf(stderr, RED"ERROR! traducir_bloque_inodo() en mi_write_f()\n"RESET);
		return FALLO;
	}

	// Leer el bloque físico para preservar datos no escritos
	if (bread(nbfisico, buf_bloque) == FALLO) {
        mi_signalSem();
		fprintf(stderr, RED"ERROR! bread() en mi_write_f()\n"RESET);
		return FALLO;
	}

    // Caso 1: un solo bloque
    if (primerBL == ultimoBL) {
        // Escribir los bytes en el bloque
        memcpy(buf_bloque + desp1, buf_original, nbytes);

        // Escribir el bloque físico modificado
        if (bwrite(nbfisico, buf_bloque) == FALLO) {
            mi_signalSem();
            fprintf(stderr, RED"ERROR! bwrite() en mi_write_f()\n"RESET);
            return FALLO;
        }

        // Actualizar la cantidad de bytes escritos
        bytesEscritos += nbytes;
        
    } else  if (primerBL < ultimoBL) {
        // Fase 1: Primer bloque lógico (BL8)
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO) {
            mi_signalSem();
            fprintf(stderr, RED"ERROR! bwrite() en mi_write_f()\n"RESET);
            return FALLO;
        }

        bytesEscritos += BLOCKSIZE - desp1;

        // Fase 2: Bloques lógicos intermedios (bloques 9, 10, 11)
        for (int bl = primerBL + 1; bl < ultimoBL; bl++) {
            nbfisico = traducir_bloque_inodo(&inodo, bl, 1);
            if (nbfisico == FALLO) {
                mi_signalSem();
                fprintf(stderr, RED"Error al traducir bloque lógico\n"RESET);
                return FALLO;
            }

            if (bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE) == FALLO) {
                mi_signalSem();
                fprintf(stderr, RED"Error al escribir bloque físico\n"RESET);
                return FALLO;
            }

            bytesEscritos += BLOCKSIZE;
        }


        // Fase 3: Último bloque lógico (bloque 12)
        /* --- leer nbloque otra vez, pero con ultimoBL --- */
            // Obtener el número de bloque físico correspondiente a ultimoBL
            nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 1);
            if (nbfisico == FALLO) {
                mi_signalSem();
                fprintf(stderr, RED"ERROR! traducir_bloque_inodo() en mi_write_f()\n"RESET);
                return FALLO;
            }

            // Leer el bloque físico para preservar datos no escritos
            if (bread(nbfisico, buf_bloque) == FALLO) {
                mi_signalSem();
                fprintf(stderr, RED"ERROR! bread() en mi_write_f()\n"RESET);
                return FALLO;
            }
        /* -------------------- */
        
        //memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1); //hay una incongruencia en el nivel 5 entre el code y el ejemplo
        memcpy(buf_bloque, buf_original + (nbytes - (desp2 - 1)), desp2 + 1); //creo que este es el correcto
        if (bwrite(nbfisico, buf_bloque) == FALLO) {
            fprintf(stderr, RED"Error al escribir bloque físico\n"RESET);
            mi_signalSem();
            return FALLO;
        }

        bytesEscritos += desp2 + 1;
    }
    	
	else{
		fprintf(stderr, "caso no posible en mi_write_f (?)\n");
	}
	
    // METAINFORMACIÓN INODO
	// Actualizar el tamaño en bytes lógico (tamEnBytesLog) si hemos escrito más allá del final del fichero
	if (offset + nbytes > inodo.tamEnBytesLog) {
		inodo.tamEnBytesLog = offset + nbytes;
		// Actualizar ctime (porque hemos actualizado campos del inodo)
		inodo.ctime = time(NULL);
	}

	// Actualizar mtime (porque hemos escrito en la zona de datos)
	inodo.mtime = time(NULL);
	

	// Escribir el inodo actualizado en el dispositivo
	if (escribir_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
		fprintf(stderr, RED"Error al escribir el inodo en mi_write_f()\n"RESET);
		return FALLO;
	}
	
	//para probar, BORRAR LUEGO SI TODO FUNCOIONA
	if (nbytes != bytesEscritos) { fprintf(stderr, RED"NBYTES Y BYTESESCRITOS NO CUADRA EN MI_WRITE_F !!!!!!!\n"RESET); }
    mi_signalSem();
    return bytesEscritos;
}

/*
Metodo mi_read_f el cual a partir de un numero de inodo, un buffer vacio, un offset de inico y el numero
de bytes que queremos escribir, este leerá el contenido del fichero y lo pasará al buff original

Este mismo metodo necesitará los mismos casos del mi_write_f para poder ejecutar los memcpy en el buff
correctamente

return:nbytes leídos

Estos irán aumentando conforme se vaya escribiendo
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    mi_waitSem();
    struct inodo inodo;
    int primerBL, ultimoBL, desp1, desp2;
    int bytesEscritos = 0;
    
    // Leer el inodo
 
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, RED"Error al leer el inodo\n"RESET);
        mi_signalSem();
        return 0;
        //return FALLO; //?
    }

    // Comprobar permisos de lectura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, RED"No hay permisos de lectura\n"RESET);
        mi_signalSem();
        return 0;
    }

	// Comprobar si offset >= tamEnBytesLog
    if (offset >= inodo.tamEnBytesLog) {
        mi_signalSem();
        return 0; // No podemos leer nada
    }

    // Ajustar nbytes si se intenta leer más allá de EOF
    if (offset + nbytes >= inodo.tamEnBytesLog) {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    // Calcular primer y último bloque lógico
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    // Calcular desplazamientos dentro de bloques
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;
	
	unsigned int nbfisico;
	char buf_bloque[BLOCKSIZE];

	// Obtener el número de bloque físico correspondiente a primerBL
	nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);
	
	// Caso 1: un solo bloque
	if (primerBL == ultimoBL) {
		if (nbfisico != -1) { //solo nos interesa si traducir_bloque_inodo nis ha devuelto un nbfisico 
			// Escribir el bloque físico modificado
			if (bread(nbfisico, buf_bloque) == FALLO) {
                mi_signalSem();
				fprintf(stderr, RED"ERROR! bwrite() en mi_write_f()\n"RESET);
				return FALLO;
			}
			
			// Escribir los bytes en el bloque
			memcpy(buf_original, buf_bloque + desp1, nbytes);
        }

		// Actualizar la cantidad de bytes escritos
		bytesEscritos += nbytes;
		
	} else  if (primerBL < ultimoBL) {
		// Fase 1: Primer bloque lógico (BL8)
		if (nbfisico != -1) {
			if (bread(nbfisico, buf_bloque) == FALLO) {
                mi_signalSem();
				fprintf(stderr, RED"ERROR! bread() en mi_read_f()\n"RESET);
				return FALLO;
			}
			memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
		}
		
		bytesEscritos += BLOCKSIZE - desp1;

		// Fase 2: Bloques lógicos intermedios (bloques 9, 10, 11)
		for (int bl = primerBL + 1; bl < ultimoBL; bl++) {
			nbfisico = traducir_bloque_inodo(&inodo, bl, 0);
			if (nbfisico != -1) {
				if (bread(nbfisico, buf_bloque) == FALLO){	
                    mi_signalSem();
					fprintf(stderr, RED"Error al escribir bloque físico\n"RESET);
					return FALLO;
				}
				memcpy(buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
			}

			bytesEscritos += BLOCKSIZE;
		}

		// Fase 3: Último bloque lógico (bloque 12)
			// Obtener el número de bloque físico correspondiente a ultimoBL
			nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 0);
		
		if (nbfisico != -1)	{
			if (bread(nbfisico, buf_bloque) == FALLO) {
                mi_signalSem();
				fprintf(stderr, RED"Error al leer bloque físico\n"RESET);
				return FALLO;
			}
			memcpy(buf_original + (nbytes - (desp2 - 1)), buf_bloque, desp2 + 1);
		}
		bytesEscritos += desp2 + 1;
	}
	
	else{
		fprintf(stderr, "caso no posible (?) en mi_read_f\n");
	}

    // METAINFORMACIÓN
	// Actualizar atime del inodo
    inodo.atime = time(NULL);

	// Escribir el inodo actualizado en el dispositivo
	if (escribir_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
		fprintf(stderr, RED"Error al escribir el inodo en mi_read_f()\n"RESET);
		return FALLO;
	}
	mi_signalSem();
	//para probar, BORRAR LUEGO SI TODO FUNCOIONA
	if (nbytes != bytesEscritos) { fprintf(stderr, RED"NBYTES Y BYTES LEIDOS NO CUADRA EN MI_READ_F !!!!!!!\n"RESET); }
    mi_signalSem();
    return bytesEscritos;
}
/*
Metodo mi_stat_f que a partir de un numero de inodo y un struct stat con sus componentes no vacias, se leerá el
inodo correspondiente a este numero de inodo para más tarde inicializarlo con los valores del struct stat

return:FALLO O EXITO
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) { //gestión de errores :D
        fprintf(stderr, RED"Error al leer el inodo\n"RESET);
        return FALLO;
    }

    //p_stat->reservado_alineacion1=indo.reservado_alineacion1[6];
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return EXITO;
}

/*
Metodo mi_chmod_f que a partir de un numero de inodo y unos permisos, se lee el inodo y se cambia los permisos por
los que se pasan por parametro, para más tarde guardar el inodo.

return:FALLO O EXITO
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    mi_waitSem();
    
    // Leer el inodo correspondiente
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
        fprintf(stderr, RED"Error al leer el inodo\n"RESET);
        return FALLO;
    }

    // Actualizar los permisos
    inodo.permisos = permisos;

    // Actualizar ctime
    inodo.ctime = time(NULL);

    // Guardar el inodo actualizado
    if (escribir_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
        fprintf(stderr, RED"Error al guardar el inodo\n"RESET);
        return FALLO;
    }
    mi_signalSem();
    // Devolver EXITO
    return EXITO;
}


/* ****** FIN NIVEL 5 ****** */


/* ----------------------------------------
                  NIVEL 6
---------------------------------------- */
/*
Metodo mi_truncar_f que a partir de un numero de inodo y un numero de bytes, se truncara el valor del fichero
des de el numero de bytes pasado por parametros.

return:numero de bloques liberados
*/
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    struct inodo inodo;
    int liberados=0;
     // Leer el inodo correspondiente
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, RED"mi_truncar_f()-> Error al leer el inodo\n"RESET);
        return FALLO;
    }

    // comparación para averiguar permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, RED"mi_truncar_f()-> No hay permisos de escritura\n"RESET);
        return FALLO;
    }
  
    if(inodo.tamEnBytesLog<nbytes){
        fprintf(stderr, RED"mi_truncar_f()-> El tamaño en bytes logicos es mas pequeño que el numero de bytes\n"RESET);
        return FALLO;
    }
    unsigned int primerBL;
    if((nbytes % BLOCKSIZE )==0 ){
        primerBL= nbytes/BLOCKSIZE;
    }
    else{
        primerBL = nbytes/BLOCKSIZE + 1;
    }
      
    liberados=liberar_bloques_inodo(primerBL,&inodo);
    inodo.mtime=time(NULL);
    inodo.ctime=time(NULL);

    inodo.tamEnBytesLog= nbytes;
    inodo.numBloquesOcupados= inodo.numBloquesOcupados- liberados;

    if(escribir_inodo(ninodo,&inodo)== FALLO){
        fprintf(stderr, RED"mi_truncar_f()-> no se pudo escribir el inodo\n"RESET);
        return FALLO;
    }

    return liberados;

}
