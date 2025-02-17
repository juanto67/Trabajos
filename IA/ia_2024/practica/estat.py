import copy

from practica import joc
from practica.joc import Accions
class Estat:

    def __init__(self,taulell,x,y,desti,parets,pes,accionesprv: Accions = None):


        if accionesprv==None:
            accionesprv=[]
        taulell[desti[0]][desti[1]]=" "
        self.taulell = taulell
        self.x = x
        self.y = y
        self.accionesprv = accionesprv
        self.desti = desti
        self.parets=parets
        self.pes=pes



    def es_meta(self) -> bool:

      return ((self.desti[0]==self.x) and (self.desti[1]==self.y))





    def genera_fill(self)-> list:
        fills= []

        posiblesX=[self.x+1,self.x-1,self.x+2,self.x-2]
        posiblesY=[self.y+1,self.y-1,self.y+2,self.y-2]
        pesos=[1,1,2,2]

        accionesPosiblesX= [(Accions.MOURE,"E"),(Accions.MOURE,"O"),(Accions.BOTAR,"E"),(Accions.BOTAR,"O")]
        accionesPosiblesY=[(Accions.MOURE,"S"),(Accions.MOURE,"N"),(Accions.BOTAR,"S"),(Accions.BOTAR,"N")]
        accionesPosiblesMX=[(Accions.POSAR_PARET,"E"),(Accions.POSAR_PARET,"O")]
        accionesPosiblesMY=[(Accions.POSAR_PARET,"S"),(Accions.POSAR_PARET,"N")]

        for pos in range(len(posiblesX)):

            previas = copy.deepcopy(self.accionesprv)

            if -1<posiblesX[pos]<=(len(self.taulell)-1) and ((posiblesX[pos],self.y) not in self.parets):

                    taula = copy.deepcopy(self.taulell)
                    taula[self.x][self.y] =" "
                    taula[posiblesX[pos]][self.y] ="O"

                    previas.append(accionesPosiblesX[pos])
                    fills.append(Estat(taula,posiblesX[pos],self.y,self.desti,self.parets,self.pes+pesos[pos],previas))
                    previas =  copy.deepcopy(self.accionesprv)


                    if pos<2:
                        taula[posiblesX[pos]][self.y] = "O"
                        previas.append(accionesPosiblesMX[pos])
                        fills.append(Estat(self.taulell,self.x,self.y,self.desti,self.parets,self.pes+4,previas))
                        previas =  copy.deepcopy(self.accionesprv)



        for pos1 in range(len(posiblesY)):

            previas = copy.deepcopy(self.accionesprv)
            if -1<posiblesY[pos1]<=(len(self.taulell[0])-1) and ((self.x,posiblesY[pos1]) not in self.parets):


                    taula = copy.deepcopy(self.taulell)
                    taula[self.x][self.y] =" "
                    taula[self.x][posiblesY[pos1]] = "O"
                    previas.append(accionesPosiblesY[pos1])

                    fills.append(Estat(taula,self.x,posiblesY[pos1],self.desti,self.parets,self.pes+pesos[pos1],previas))

                    previas =  copy.deepcopy(self.accionesprv)

                    if pos1 < 2:
                        # modificar AQUÍ SE PONEN MUROS
                        previas.append(accionesPosiblesMY[pos1])

                        taula[self.x][posiblesY[pos1]] = "O"
                        fills.append(Estat(taula, self.x, self.y,self.desti,self.parets,self.pes+4,previas))
                        previas =  copy.deepcopy(self.accionesprv)
        if fills is None:
            return None
        return fills

    def __hash__(self):
        dato= tuple(self.parets)

        return hash(str(self.x)+","+str(self.y)+","+str(hash(dato)) )


    def __eq__(self, other):

        return hash(self) == hash(other)


    def __str__(self):
        return "X:{self.x}, Y={self.y}"

    def total(self) -> int:
        return self.calc_heuristica() + self.pes


    def calc_heuristica(self) -> int:
        return abs(self.x - self.desti[0]) + abs(self.y - self.desti[1])

    def __lt__(self, other):
        return False