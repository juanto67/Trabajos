import copy

from scipy.special import y1_zeros

from practica import joc
from practica.joc import Accions
class Estat:

    def __init__(self,taulell,x1,y1,x2,y2,desti,parets,turno,accionesprv: tuple[Accions, str] = None):


        if accionesprv==None:
            accionesprv=[]
        taulell[desti[0]][desti[1]]=" "
        self.taulell = taulell
        self.turno = turno



        self.x1=x1
        self.y1=y1

        self.x2=x2
        self.y2=y2
        if self.turno is True:
            self.x=self.x1
            self.y=self.y1
        else:
            self.x=self.x2
            self.y=self.y2
        self.accionesprv = accionesprv
        self.desti = desti
        self.parets=parets




    def es_meta(self,prf,max) -> bool:

      return (((self.desti[0]==self.x) and (self.desti[1]==self.y)) or prf>=max)





    def genera_fill(self)-> list:
        fills= []

        posiblesX=[self.x+1,self.x-1,self.x+2,self.x-2]
        posiblesY=[self.y+1,self.y-1,self.y+2,self.y-2]



        accionesPosiblesX= [(Accions.MOURE,"E"),(Accions.MOURE,"O"),(Accions.BOTAR,"E"),(Accions.BOTAR,"O")]
        accionesPosiblesY=[(Accions.MOURE,"S"),(Accions.MOURE,"N"),(Accions.BOTAR,"S"),(Accions.BOTAR,"N")]
        accionesPosiblesMX=[(Accions.POSAR_PARET,"E"),(Accions.POSAR_PARET,"O")]
        accionesPosiblesMY=[(Accions.POSAR_PARET,"S"),(Accions.POSAR_PARET,"N")]

        for pos in range(len(posiblesX)):



            if -1<posiblesX[pos]<=(len(self.taulell)-1) and ((posiblesX[pos],self.y) not in self.parets) and (self.taulell[posiblesX[pos]][self.y] != "O"):
                    nou_estat = copy.copy(self)
                    nou_estat.taulell = copy.copy(self.taulell)

                    nou_estat.taulell[self.x][self.y] =" "
                    nou_estat.taulell[posiblesX[pos]][self.y] ="O"

                    if self.turno is True:
                        fills.append(Estat(nou_estat.taulell,posiblesX[pos],self.y1,self.x2,self.y2,self.desti,self.parets,not self.turno,accionesPosiblesX[pos]))
                    else:
                        fills.append(
                            Estat(nou_estat.taulell, self.x1,self.y1,posiblesX[pos],self.y2, self.desti, self.parets,
                                  not self.turno, accionesPosiblesX[pos]))


                    if pos<2 and (posiblesX[pos]!=self.desti[0] or self.y!=self.desti[1]):
                        nou_estat = copy.copy(self)
                        nou_estat.taulell = copy.copy(self.taulell)
                        nou_estat.taulell[posiblesX[pos]][self.y] = "O"

                        fills.append(Estat(nou_estat.taulell,self.x1,self.y1,self.x2,self.y2,self.desti,self.parets,not self.turno,accionesPosiblesMX[pos]))




        for pos1 in range(len(posiblesY)):


            if -1<posiblesY[pos1]<=(len(self.taulell[0])-1) and ((self.x,posiblesY[pos1]) not in self.parets)and (self.taulell[self.x][posiblesY[pos1]] != "O"):

                    nou_estat = copy.copy(self)
                    nou_estat.taulell = copy.copy(self.taulell)
                    nou_estat.taulell [self.x][self.y] =" "
                    nou_estat.taulell [self.x][posiblesY[pos1]] = "O"

                    if self.turno is True:
                        fills.append(
                            Estat(nou_estat.taulell, self.x1, posiblesY[pos1], self.x2, self.y2, self.desti, self.parets, not self.turno, accionesPosiblesY[pos1]))
                    else:
                        fills.append(
                            Estat(nou_estat.taulell, self.x1, self.y1, self.x2, posiblesY[pos1], self.desti, self.parets,
                                  not self.turno, accionesPosiblesY[pos1]))



                    if pos1 < 2 and (posiblesY[pos1]!=self.desti[1] or self.x!=self.desti[0]):
                        # modificar AQUÍ SE PONEN MUROS
                        nou_estat = copy.copy(self)
                        nou_estat.taulell = copy.copy(self.taulell)

                        nou_estat.taulell[self.x][posiblesY[pos1]] = "O"
                        fills.append(Estat(nou_estat.taulell,self.x1, self.y1, self.x2, self.y2,self.desti,self.parets,not self.turno,accionesPosiblesMY[pos1]))

        if fills is None:
            return None
        return fills

    def __hash__(self):
        dato= tuple(self.parets)
        if self.turno:
           return hash(str(self.x2)+","+str(self.y2)+str(hash(dato)))
        else:
            return hash(str(self.x1)+","+str(self.y1)+str(hash(dato)))


    def __eq__(self, other):

        return hash(self) == hash(other)


    def __str__(self):
        return "X:{self.x}, Y={self.y}"



    def calc_heuristica(self) -> int:
        return (abs(self.x1 - self.desti[0]) + abs(self.y1 - self.desti[1])) + \
            (abs(self.x2 - self.desti[0]) + abs(self.y2 - self.desti[1]))



    def __lt__(self, other):
        return False