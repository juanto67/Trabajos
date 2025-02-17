""" Fitxer que conté l'agent barca en profunditat.

S'ha d'implementar el mètode:
    actua()
"""
from quiques.agent import Barca
from quiques.estat import Estat
from quiques.joc import AccionsBarca


class BarcaProfunditat(Barca):
    def _init_(self):
        super(BarcaProfunditat, self)._init_()
        self.__per_visitar = None
        self.__visitats = None
        self.__cami_exit = None

    def cerca(self, estat_inicial: Estat) -> bool:
        self.__per_visitar = []
        self.__visitats = set()
        exit = False

        self.__per_visitar.append(estat_inicial)
        while self.__per_visitar:
            estat_actual = self.__per_visitar.pop(-1)

            if estat_actual in self.__visitats or not estat_actual.es_segur():
                continue

            if estat_actual.es_meta():
                break

            for f in estat_actual.genera_fill():
                self.__per_visitar.append(f)

            self.__visitats.add(estat_actual)

        if estat_actual.es_meta():
            self.__cami_exit = estat_actual.cami
            exit = True

        return exit

    def actua(self, percepcio: dict) -> AccionsBarca | tuple[AccionsBarca, (int, int)]:
        if self.__cami_exit is None:
            estat_inicial = Estat(
                local_barca=percepcio["Lloc"],
                llops_esq=percepcio["Llop Esq"],
                polls_esq=percepcio["Poll Esq"],
            )

            self.cerca(estat_inicial)

        if self.__cami_exit:
            quiques, llops = self.__cami_exit.pop(-1)

            return AccionsBarca.MOURE, (quiques, llops)
        else:
            return AccionsBarca.ATURAR