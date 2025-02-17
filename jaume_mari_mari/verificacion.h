/* ----------------------------------------
AUTORES:
    Antoni Jaume Lemesev
    Antonio Marí González
    Juan Marí González
---------------------------------------- */
//verificacion.h
#include "simulacion.h"


struct INFORMACION {
  int pid;
  unsigned int nEscrituras; //validadas 
  struct REGISTRO PrimeraEscritura;
  struct REGISTRO UltimaEscritura;
  struct REGISTRO MenorPosicion;
  struct REGISTRO MayorPosicion;
};
