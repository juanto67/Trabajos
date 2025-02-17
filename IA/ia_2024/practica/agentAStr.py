import time
from queue import PriorityQueue


from practica import joc
from practica.joc import Accions
from practica.estat import Estat

class Viatger(joc.Viatger):
    def __init__(self, *args, **kwargs):
        super(Viatger, self).__init__(*args, **kwargs)
        self.__oberts = None
        self.__tancats = None
        self.__accions = None
        self.__args = args
        self.tiempoej = None
        self.tiempoIni = None


    def pinta(self, display):
        pass

    def cerca(self, estat_inicial):

        self.__oberts = PriorityQueue()
        self.__tancats = set()

        self.__oberts.put((estat_inicial.total(), estat_inicial))

        actual = None
        while not self.__oberts.empty():
            _, actual = self.__oberts.get()
            if actual in self.__tancats:
                continue

            if actual.es_meta():
                break

            estats_fills = actual.genera_fill()
            if estats_fills is not None:
                for estat_f in estats_fills:

                    self.__oberts.put((estat_f.total(), estat_f))

            self.__tancats.add(actual)

        if actual.es_meta():
            self.tiempoej = time.time() - self.tiempoIni
            print("Tiempo ejecucion: " + str(self.tiempoej))
            print("Abiertos: " + str(self.__oberts.qsize()))
            print("Visitados: " + str(len(self.__tancats)))

            self.__accions = actual.accionesprv

    def actua(self, percepcio: dict) -> Accions | tuple[Accions, str]:

        ag = percepcio["AGENTS"]
        self.tiempoIni = time.time()
        x, y = ag[self.__args[0]]
        estat_inicial = Estat(percepcio["TAULELL"], x, y, percepcio["DESTI"], percepcio["PARETS"],0)
        if self.__accions == None:
            self.cerca(estat_inicial)


        if self.__accions:

            Accio = self.__accions.pop(0)

            return Accio
        else:
            return Accions.ESPERAR