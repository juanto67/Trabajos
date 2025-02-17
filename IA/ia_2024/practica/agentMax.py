import time
from operator import truediv

from practica import joc
from practica.joc import Accions
from practica.estatMX import Estat

class Viatger(joc.Viatger):
    turno=False
    inicio=True
    def __init__(self, *args, **kwargs):
        super(Viatger, self).__init__(*args, **kwargs)
        self.__args = args
        self.__kwargs = kwargs
        self.__cami = None
        self.__visitats = None
        self.__profundidad= 0
        dato=kwargs["mida_taulell"]
        self.__profundidadmax= dato[0]+dato[1]
        self.tiempoej = None
        self.tiempoIni = None

    def cerca(self, estat: Estat, alpha, beta, torn_max=True):
        if estat.es_meta(self.__profundidad,self.__profundidadmax):
            return estat, (estat.calc_heuristica())

        puntuacio_fills = []

        for fill in estat.genera_fill():
                self.__profundidad += 1
                if fill is not None:
                    if fill not in self.__visitats:

                        punt_fill = self.cerca(fill, alpha, beta, not torn_max)

                        if punt_fill is None:
                            continue
                        if torn_max:
                            alpha = max(alpha, punt_fill[1])
                        else:
                            beta = min(beta, punt_fill[1])

                        if alpha >= beta:
                            break

                        self.__visitats[fill] = punt_fill
                    puntuacio_fills.append(self.__visitats[fill])

        puntuacio_fills = sorted(puntuacio_fills, key=lambda x: x[1])
        if len(puntuacio_fills)>0:
            if torn_max:
                return puntuacio_fills[0]
            else:
                return puntuacio_fills[-1]

    def pinta(self, display):
        pass

    def actua(self, percepcio: dict) -> Accions | tuple[Accions, str]:


        self.__visitats = dict()

        ag=percepcio["AGENTS"]
        x,y = ag["Juan"]
        x1,y1=ag["Antonio"]
        Viatger.turno=not Viatger.turno
        estat_inicial = Estat(percepcio["TAULELL"],x,y,x1,y1, percepcio["DESTI"], percepcio["PARETS"],Viatger.turno)

        res=self.cerca(estat_inicial, alpha=-float('inf'), beta=float('inf'))
        self.__profundidad=0
        if isinstance(res, tuple) and res[0].accionesprv is not None and len(res[0].accionesprv) > 0:
            solucio, punt = res

            return solucio.accionesprv
        else:


            return Accions.ESPERAR




