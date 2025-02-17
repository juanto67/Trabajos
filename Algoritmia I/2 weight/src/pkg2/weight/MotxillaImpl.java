/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package pkg2.weight;

/**
 *
 * @author antoni
 */
public class MotxillaImpl implements Motxilla {

    private double maxp = 0;

    /**
     *  Método que realiza el problema de la mochila de manera recursiva.
     *  
     *  En este método únicamente inicializamos las variables necesarias
     *  para el backtracking recursivo. Después de ejecutar el método
     *  recursivo usaremos el método resultado, para crear la solución de
     *  la manera que se pide en el enunciado.
     * 
     * @param a array de elemntos mochila, de donde sacaremos los weight y profit
     * @param W1 peso a conseguir pedido por el usuario
     * @param W2 peso a conseguir pedido por el usuario
     * @return un array de elementos mochila con los elementos que dan
     * mejor profit y los weight indicado.s
     */
    @Override
    public ElementMotxilla[] recursiu(ElementMotxilla[] a, double W1, double W2) {
        int[] solucion = new int[a.length];
        int[] t = new int[a.length];
        maxp = 0;
        mochila(a, W1, W2, t, 0, solucion, 0, 0, 0,rmd(0,a));
        return resultado(a, solucion);
    }

    /**
     * ---------------------Explicación general-------------------------------
     * Método que realiza el método de la mochila de manera recursiva. En esta
     * variación del método de la mochila cada elemento tiene dos pesos, se busca 
     * los elementos mochila cuyos weigth 1 acumulados den el peso(o menor) indicado por el usuario, 
     * lo mismo para el weigth 2. Pero, para ser solución debe tener el mayor profit
     * de las soluciones válidas.
     * 
     * -----------------------Explicación método--------------------------------
     * En este método(y en el iterativo) usamos el array t como auxiliar para 
     * realizar la solución. Inicializaremos cada elemento de t a -1, de tal
     * manera que cuando entremos en el while lo aumentaremos y será 0, hacemos
     * operaciones con 0, para comprobar si la suma de los weight anterior
     * ya era una solución válida. En el caso de que no lo sea, sumaremos el
     * weight y el profit del elemento de a en el que estamos(siguiente iteración)
     * 
     * --------------------------Análisis de casos-----------------------------
     * En el primer if, comprobamos si es una solución válida y si está en el 
     * final del arbol. Si se cumplen estas condiciones comprobamos si es una
     * solución con un profit mejor al de la anterior solución, si se cumple,
     * copiaremos el array t(con los 1) en el array solución.
     * 
     * En el segundo if, comprobamos si encontramos una solución válida, pero 
     * no estamos en el final, significa que pueden haber siguientes elementos
     * con los que sumemos los weight y estén dentro de la solución, por tanto
     * comprobamos con el siguiente elemento.
     * 
     * Si ninguna de estas condiciones se producen, significa que no es una
     * solución válida, por tanto quitamos los weights y profit sumados a la
     * acumulación. Finalmente, si salimos del while, significará que no se cumplió
     * ninguna de las condiciones después comprobarlas, por consiguiente pondremos
     * t[k] a -1 indicando que no es una solución.
     * 
     * 
     * ----------------------------Complejidad--------------------------------
     * Usaremos el teorema de la substración, ya que cuando llamamos recursivamente
     * vamos al siguiente elemento del array. Los valores serían:
     * a = 2 (por el while)
     * c = 1
     * alpha = 1 (porque el System.arraycopy tiene una complejidad de O(n))
     * 
     * Lo que nos da finalmente 2^n.
     * 
     * ----------------------------Poda--------------------------------
     * Tenemos una función llamada rmd que calcula los valores de weight1 y weight2
     * a partir de un indice, que meteremos en un array donde la posición 0 es
     * el weight1 y la 1 el weight2. Con el rmd de todo el array iremos restando si añadimos
     * un valor y sumandolo si no lo añadimos, entonces tendremos la suma de los siguientes
     * valores de weights, con eso lo sumaremos con el valor que de weight que estamos utilizando
     * y comprobaremos que es más grande que el valor que buscamos, es decir, si todavía
     * tenemos valores que llegen al valor que deseado.
     *
     * 
     * @param a array del que sacaremos los pesos y profit
     * @param W1 peso indicado por el usuario
     * @param W2 peso indicado por el usuario
     * @param t array donde guardaremos la solución para después copiarla en
     * el array de soluciones
     * @param k indice que indica el elemento en el que nos encontramos
     * @param solucion array con las soluciones
     * @param weight1 peso acumulado
     * @param weight2 peso acumulado
     * @param profit profit acumulado
     */
    private void mochila(ElementMotxilla[] a, double W1, double W2, int[] t, int k, int[] solucion, double weight1, double weight2, double profit, double [] rd) {
        t[k] = -1;
        
        while (t[k] < 1) {
            t[k]++;
            weight1 += a[k].getWeigth1() * t[k];
            weight2 += a[k].getWeigth2() * t[k];
            profit += a[k].getProfit() * t[k];
            rd[0] -= a[k].getWeigth1() * t[k];
            rd[1] -= a[k].getWeigth2() * t[k];
            if (weight1 <= W1 && weight2 <= W2 && (k == t.length - 1)&&(profit>maxp)&&((weight1+rd[0])>W1)&&((weight2+rd[1])>W2)) {
                
                    maxp = profit;
                    System.arraycopy(t, 0, solucion, 0, t.length);
                
            } else if (weight1 <= W1 && weight2 <= W2 && (k < t.length - 1)&&((weight1+rd[0])>W1)&&((weight2+rd[1])>W2)) {
                
                mochila(a, W1, W2, t, k + 1, solucion, weight1, weight2, profit,rd);
            }
            weight1 -= a[k].getWeigth1() * t[k];
            weight2 -= a[k].getWeigth2() * t[k];
            profit -= a[k].getProfit() * t[k];
            rd[0]+= a[k].getWeigth1() * t[k];
            rd[1]+= a[k].getWeigth2() * t[k];
        }
        
        t[k] = -1;
    }

    /**
     * 
     * ---------------------Explicación general-------------------------------
     * Método que realiza el método de la mochila de manera iterativa. En esta
     * variación del método de la mochila cada elemento tiene dos pesos, se busca 
     * los elementos mochila cuyos weigth 1 acumulados den el peso(o menor) indicado por el usuario, 
     * lo mismo para el weigth 2. Pero, para ser solución debe tener el mayor profit
     * de las soluciones válidas.
     * 
     * -----------------------Explicación método--------------------------------
     * En este método(y en el recursivo) usamos el array t como auxiliar para 
     * realizar la solución. Inicializaremos cada elemento de t a -1, de tal
     * manera que cuando entremos en el while lo aumentaremos y será 0, hacemos
     * operaciones con 0, para comprobar si la suma de los weight anterior
     * ya era una solución válida. En el caso de que no lo sea, sumaremos el
     * weight y el profit del elemento de a en el que estamos(siguiente iteración)
     * 
     * --------------------------Análisis de casos-----------------------------
     * En el segundo if, comprobamos si es una solución válida y si está en el 
     * final del arbol. Si se cumplen estas condiciones comprobamos si es una
     * solución con un profit mejor al de la anterior solución, si se cumple,
     * copiaremos el array t(con los 1) en el array solución.
     * 
     * En el else if al primer if, comprobamos si encontramos una solución válida, pero 
     * no estamos en el final, significa que pueden haber siguientes elementos
     * con los que sumemos los weight y estén dentro de la solución, por tanto
     * comprobamos con el siguiente elemento.
     * 
     * Si ninguna de estas condiciones se producen, significa que no es una
     * solución válida, por tanto quitamos los weights y profit sumados a la
     * acumulación. Finalmente, si salimos del while, significará que no se cumplió
     * ninguna de las condiciones después comprobarlas, por consiguiente pondremos
     * t[k] a -1 indicando que no es una solución.
     * 
     * Finalmente, cuando se acabe el while, significará que ya hemos analizado
     * cada valor del array, por tanto ejecutaremos el método resultado, que
     * devolverá el resultado en el formato pedido.
     * ----------------------------Complejidad--------------------------------
     * Si se ha pasado el algoritmo recursivo a iterativo correctamente tendrá
     * la misma complejidad que el recursivo, entonces O(2^n)
     * 
     * ----------------------------Poda--------------------------------
     * Tenemos una función llamada rmd que calcula los valores de weight1 y weight2
     * a partir de un indice, que meteremos en un array donde la posición 0 es
     * el weight1 y la 1 el weight2. Con el rmd de todo el array iremos restando si añadimos
     * un valor y sumandolo si no lo añadimos, entonces tendremos la suma de los siguientes
     * valores de weights, con eso lo sumaremos con el valor que de weight que estamos utilizando
     * y comprobaremos que es más grande que el valor que buscamos, es decir, si todavía
     * tenemos valores que llegen al valor que deseado.
     * 
     * @param a array del que sacaremos los pesos y profit
     * @param W1 peso indicado
     * @param W2 peso indicado
     * @return devuelve un array con los elementos que forman la solución válida.
     */
    @Override
    public ElementMotxilla[] iteratiu(ElementMotxilla[] a, double W1, double W2) {
        
        int[] solucion = new int[a.length];
        int[] t = new int[a.length];
        maxp = 0.0;
        double[] rd=rmd(0,a);
        for (int i = 0; i < t.length; i++) {
            t[i] = -1;
        }
        int k = 0;
        double weight1 = 0.0;
        double weight2 = 0.0;
        double profit = 0.0;

        while (k >= 0) {
            if (t[k] < 1) {
                t[k]++;
                weight1 += a[k].getWeigth1() * t[k];
                weight2 += a[k].getWeigth2() * t[k];
                profit += a[k].getProfit() * t[k];
                rd[0] -= a[k].getWeigth1() * t[k];
                rd[1] -= a[k].getWeigth2() * t[k];       
                if (weight1 <= W1 && weight2 <= W2 && (k == t.length - 1)&&((weight1+rd[0])>W1)&&((weight2+rd[1])>W2)) {
                    if (profit > maxp) {
                        maxp = profit;
                       
                        System.arraycopy(t, 0, solucion, 0, t.length);
                    }
                    } else if (weight1 <= W1 && weight2 <= W2 && (k < t.length - 1)&&((weight1+rd[0])>W1)&&((weight2+rd[1])>W2)) {
                        k++;
                    }
                } else {
                    weight1 -= a[k].getWeigth1() * t[k];
                    weight2 -= a[k].getWeigth2() * t[k];
                    profit -= a[k].getProfit() * t[k];
                    rd[0] += a[k].getWeigth1() * t[k];
                    rd[1] += a[k].getWeigth2() * t[k];
                    t[k] = -1;
                    k--;
                }

            }
                return resultado(a, solucion);
        }

    
/*
     * Método que detecta los 1 en la solución, y mete los elementos mochila asociados
     * dentro de un array de elementos mochila con la longitud de las soluciones
     * existentes.
     * @param a array del que sacaremos los pesos y profit
     * @param solucion array con los 1 que indican la solución válida
     * 
     * @return devuelve un array con los elementos que forman la solución válida.
     */
    
    private ElementMotxilla[] resultado(ElementMotxilla[] a, int[] solucion) {
        int unos = 0;
        for (int i1 = 0; i1 < solucion.length; i1++) {
            if (solucion[i1] == 1) {
                unos++;
            }
        }
        ElementMotxilla[] sol = new ElementMotxilla[unos];
        int i2 = 0;
        for (int i = 0; i < solucion.length; i++) {
            if (solucion[i] == 1) {
                sol[i2] = a[i];
                i2++;
            }
        }
        return sol;
    }
    
    private double[] rmd(int k,ElementMotxilla[] a){
        double []rd=new double[2];
        for(int i=k;i<a.length;i++){
            rd[0]+=a[i].getWeigth1();
            rd[1]+=a[i].getWeigth2();
        }
        return rd;
    }
}
