from practica import joc
from practica import agentAStr,agent,agentMax



def main():
    mida = (6,6)

    agents = [
        agentAStr.Viatger("Juan", mida_taulell=mida),

    ]

    lab = joc.Laberint(agents, mida_taulell=mida)

    lab.comencar()


if __name__ == "__main__":
    main()
