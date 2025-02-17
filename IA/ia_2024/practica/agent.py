import time

from practica import joc
from practica.joc import Accions
from practica.estat import Estat

class Viatger(joc.Viatger):
    def __init__(self, *args, **kwargs):
        super(Viatger, self).__init__(*args, **kwargs)
        self.__per_visitar=None
        self.__visitats=None
        self.__cami_exit=None
        self.__args=args
        self.tiempoej=None
        self.tiempoIni=None



    def pinta(self, display):
        pass

    def cerca(self, estat_inicial: Estat) -> bool:
        self.__per_visitar = []
        self.__visitats = set()

        exit = False

        self.__per_visitar.append(estat_inicial)
        while self.__per_visitar:
            estat_actual = self.__per_visitar.pop(-1)

            if estat_actual in self.__visitats:
                continue

            if estat_actual.es_meta():
                break

            f1=estat_actual.genera_fill()
            self.__visitats.add(estat_actual)

            if f1 is not None:

                for f in f1:
                    self.__per_visitar.append(f)



        if estat_actual.es_meta():
            self.tiempoej=time.time()-self.tiempoIni
            print("Tiempo ejecucion: "+str(self.tiempoej))
            print("Abiertos: "+str(len(self.__per_visitar)))
            print("Visitados: "+str(len(self.__visitats)))
            self.__cami_exit = estat_actual.accionesprv
            exit = True

        return exit

    def comprobar(self,estado_actual: Estat) -> bool:
        for f in self.__visitats:
            if estado_actual.x == f.x and estado_actual.y == f.y:
                return True
        return False

    def actua(self, percepcio: dict) -> Accions | tuple[Accions, str]:

        self.tiempoIni=time.time()
        ag=percepcio["AGENTS"]
        x,y=ag[self.__args[0]]
        estat_inicial = Estat(percepcio["TAULELL"], x, y, percepcio["DESTI"], percepcio["PARETS"],0)
        if self.__cami_exit == None:


            self.cerca(estat_inicial)

        if self.__cami_exit:


           Accio= self.__cami_exit.pop(0)


           return Accio
        else:
            return Accions.ESPERAR
