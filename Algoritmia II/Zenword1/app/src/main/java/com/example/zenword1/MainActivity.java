package com.example.zenword1;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Color;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Button;
import android.os.Bundle;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.TreeSet;

public class MainActivity extends AppCompatActivity {

    //HASHMAP de todas las palabras del fichero con su respectiva palabra con acento
    //utilizamos hash map ya que su complejidad es O(1) porque no necesiutamos tenerlas ordenadas
    private HashMap<String, String> palabras_validas;

    //HashMap con todas las palabras que son posibles soluciones de cada longitud
    //utilizamos un hash por las mismas razones anteriores
    private HashMap<Integer, HashSet<String>> palabra_solucion = new HashMap<Integer, HashSet<String>>();
    //HashMap con las 5 palabras que buscamos
    private HashMap<Integer, String> palabra_oculta = new HashMap<Integer, String>();
    //HashSet con todas las soluciones posibles
    private HashSet<String> soluciones = new HashSet();
    //Conjunto soluciones encontradas utilizamos un tree prque necesitamos las palabras ordenadas alfabeticamente
    private TreeSet<String> encontradas = new TreeSet<String>();
    //HasMap con todas las letras del circulo y el numero de apariciones de estas
    private HashMap<Character, Integer> letrasdisponibles = new HashMap<Character, Integer>();
    //HasMap de todas las palabras del fichero pero separadas por la longuitud
    private HashMap<Integer, HashSet<String>> palabras_longitud = new HashMap<Integer, HashSet<String>>();
    //TextView que tenemos arriba de la pantalla y indica
    // las palabras que hemos acertado
    private TextView encertado;
    //El texto que pondremos en el texView anterior
    private String printEncertado;

    //La longitud de la palabra más grande elegida
    private int wordLenght = 0;
    //  Declaración de todos los Botones
    private Button boton1;
    private Button boton2;
    private Button boton3;
    private Button boton4;
    private Button boton5;
    private Button boton6;
    private Button boton7;
    // Arbol en el que guardaremos todas las palabras
    //validas que introduzca el usuario durante una partida
    private TreeSet<String> acertadas1=new TreeSet<>();
    // Indicará las palabras posibles a acertar en
    // una partida
    private int posibles;

    //Array de botones, el cual contendrá todos los botones
    //y con el que trabajaremos principalmente
    private Button botones[] = new Button[7];
    //Array en el que meteremos los id de los Arrays de TextView
    private int id[];
    //Entero que indica las palabras acertadas durante una partida
    private int acertadas = 0;
    //indica algo parecido a lo anterior, simplemente se actualiza cuando
    // usas un bonus
    private int acertadasbonus=0;
    //array de colores usados para cambiar colores cuando creamos una nueva partida
    private int colores[] = {Color.BLUE, Color.GREEN, Color.GRAY, Color.YELLOW, Color.MAGENTA};
    //string que contendrá las palabras acertadas durante una partida
    //a la hora de presionar enviar
    private String palabrasavisualizar = "";
    ///     Declaracion de todos los arrays de TextView que utilizaremos
    private TextView palabra1[];
    private TextView palabra2[];
    private TextView palabra3[];

    private TextView palabra4[];

    private TextView palabra5[];
    //array de arrays con todos los textView y con el que trabajaremos
    //principalmente.
    private TextView palabras[][] = new TextView[5][1];
    /*
	Metodo oncreate que se ejecuta unicamente cuando se abre la aplicación en el que inicializaremos los botones
	y las palabras validas como también las palabras longuitud
	*/
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        palabras_validas = new HashMap<String, String>();

        ConstraintLayout layout = findViewById(R.id.background);
        setContentView(R.layout.activity_main);
        boton1 = (Button) findViewById(R.id.letra1);
        boton2 = findViewById(R.id.letra2);
        boton3 = findViewById(R.id.letra3);
        boton4 = findViewById(R.id.letra4);
        boton5 = findViewById(R.id.letra5);
        boton6 = findViewById(R.id.letra6);
        boton7 = findViewById(R.id.letra7);
        for (int i = 0; i < botones.length; i++) {
            switch (i) {
                case 0:
                    botones[i] = boton1;
                    break;
                case 1:
                    botones[i] = boton2;
                    break;
                case 2:
                    botones[i] = boton3;
                    break;
                case 3:
                    botones[i] = boton4;
                    break;
                case 4:
                    botones[i] = boton5;
                    break;
                case 5:
                    botones[i] = boton6;
                    break;
                case 6:
                    botones[i] = boton7;
                    break;
            }
        }
        //Metodo que inicializa palabras validas y palabras longitud
        metodo_iniciar();
        //Metodo que reinicia una partida
        iniciarTodo();



    }
    /*
	Metodo iniciar que inicializar los hashmap mencionados anteriormente a partir del fichero de palabras
	*/
    private void metodo_iniciar() {
        InputStream is = getResources().openRawResource(R.raw.palabras);
        BufferedReader r = new BufferedReader(new InputStreamReader(is));
        HashSet<String> pal3 = new HashSet<>();
        HashSet<String> pal4 = new HashSet<>();
        HashSet<String> pal5 = new HashSet<>();
        HashSet<String> pal6 = new HashSet<>();
        HashSet<String> pal7 = new HashSet<>();
        String line = "";
        try {
            // Lee línea por línea hasta que no haya más líneas
            while ((line = r.readLine()) != null) {
                //Utilizamos un tokenizer para separar por el ;
                StringTokenizer tokenizer = new StringTokenizer(line, ";");
                String i1 = tokenizer.nextToken();
                String i2 = tokenizer.nextToken();
                palabras_validas.put(i2, i1);
                //Dependiendo de cada logitud lo metemos en un conjunto diferente
                switch (i2.length()) {
                    case 3:
                        pal3.add(i2);
                        break;
                    case 4:
                        pal4.add(i2);
                        break;
                    case 5:
                        pal5.add(i2);
                        break;
                    case 6:
                        pal6.add(i2);
                        break;
                    case 7:
                        pal7.add(i2);
                        break;
                }
            }
            //Añadimos todos los conjuntos
            palabras_longitud.put(3, pal3);
            palabras_longitud.put(4, pal4);
            palabras_longitud.put(5, pal5);
            palabras_longitud.put(6, pal6);
            palabras_longitud.put(7, pal7);
        } catch (IOException e) {

        } finally {
            try {
                // Cierra el BufferedReader y el InputStream
                r.close();
                is.close();
            } catch (IOException e) {


            }
        }
    }
    /*
        Boton clear para vaciar el texto metido
        */
    public void ClearPalabra(View view) {
        TextView palabraEncriptada = findViewById(R.id.PalabraEnc);
        palabraEncriptada.setText("");

        for (int i = 0; i < wordLenght; i++) {

                botones[i].setEnabled(true);

        }
    }
    /*
	Metodo que al presionar un boton de una letra, meteremos esa letra en el texto
	*/

    public void setLletra(View view) {
        // Cogemos el boton que hemos presionado
        Button btn = (Button) view;

        String lletra = btn.getText().toString();
        TextView palabraEncriptada = findViewById(R.id.PalabraEnc);
        //Cogemos las letras anteriores que teniamos y le añadimos la presionada
        String datoPalabraEnc = palabraEncriptada.getText().toString();
        datoPalabraEnc = datoPalabraEnc + lletra;
        //Deshabilitamos el boton presionado
        palabraEncriptada.setText(datoPalabraEnc);
        btn.setEnabled(false);


    }
    /*
    Metodo para intercambiar todas las letras a una posicion diferente
    */
    public void shuffle(View view) {
        Random generador= new Random();
        int i = wordLenght-1;
        //Elegimos una posicion nueva totalmente random
            for (; i > 0; i--) {

                int generado = generador.nextInt(i+1);

                String texto = botones[i].getText().toString();
                String texto2 = botones[generado].getText().toString();

                botones[i].setText(texto2);
                botones[generado].setText(texto);

            }

        }


    /*
    Metodo que muestra en una nueva pestaña las palabras acertadas en la partida
    */
    public void bonus(View view) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        String datos="";
        builder.setTitle("Encertades");
        Iterator it=acertadas1.iterator();
        while (it.hasNext()){
            datos=datos+it.next()+",";
        }
        builder.setMessage(datos);
        // Un bot´o OK per tancar la finestra
        builder.setPositiveButton("OK", null);
        String textoView;
        // Mostrar l’AlertDialog a la pantalla
        AlertDialog dialog = builder.create();

        dialog.show();


    }
    /*
        Metodo ayuda si el numero de acertadas es 5 o mayor visualizaremos la primera letra de una palabra oculta
        */
    public void ayuda(View view) {
        String textoView;
        if (acertadasbonus>=5){
            Random r=new Random();
            int generado;
            boolean []cierto={false,false,false,false,false};
            //Primero comprobamos si el array de textView
            //en esa posicion es null, porque hay casos donde
            //no rellenamos todos, ya que no hay suficientes
            //palabras, posteriormente vemos si tiene contenido
            //el primer textView del array.
            for(int x=0;x<5;x++){
                if(palabras[x]!=null) {
                    textoView = palabras[x][0].getText().toString();
                    if (textoView.compareTo("") == 0) {
                        cierto[x] = true;
                    }
                }

            }
            generado=r.nextInt(5);
            //Visualizamos la primera letra de una palabra oculta random
            while (cierto[generado]==false ){
                generado=r.nextInt(5);

            }
            mostraPrimeraLletra(palabra_oculta.get(generado+1),generado+1);
            acertadasbonus=0;
            mostraMissatge("Has usat el teu bonus, ara el teus punts son:"+acertadasbonus,true);

        }

        }




    /*
	Metodo que a partir de un id guia y el numero de letras de la palabra ccreamos un array de TextView
	en la posicion de la guia correspondiente y con el numero ed letras asignado
	*/

    public TextView[] crearFilaTextViews(int guia, int lletres) {
        id = new int[lletres];

        ConstraintLayout layout = findViewById(R.id.background);

        DisplayMetrics metrics = new DisplayMetrics();

        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        float widthDisplay = metrics.widthPixels - (10 * (lletres + 1));
        float heightDisplay = metrics.heightPixels;

        float width = (float) ((widthDisplay / 7));
        int margen = (int) ((widthDisplay - (width * lletres)) / 2);
        float height = width;
        ConstraintSet constraintSet = new ConstraintSet();
        TextView[] textView = new TextView[lletres];

        for (int i = 0; i < lletres; i++) {
            //Conseguimos un id
            id[i] = View.generateViewId();
            textView[i] = new TextView(this);
            //Inicializamos el TextView
            textView[i].setId(id[i]);
            textView[i].setText(null);
            textView[i].setBackgroundColor(Color.BLUE);

            //Lo metemos en el layout
            layout.addView(textView[i]);
            // Establecemos las dimensiones del TextView
            constraintSet.constrainWidth(textView[i].getId(), (int) width);
            constraintSet.constrainHeight(textView[i].getId(), (int) height);

            constraintSet.connect(
                    textView[i].getId(),
                    ConstraintSet.TOP,
                    guia,
                    ConstraintSet.TOP,
                    10);
            //Si es el primero lo metemo en el medio del layout
            if (i == 0) {
                constraintSet.connect(
                        textView[i].getId(),
                        ConstraintSet.START,
                        layout.getId(),
                        ConstraintSet.START,
                        margen);
            } else {
                //Sino lo conectamos al anterior
                constraintSet.connect(
                        textView[i].getId(),
                        ConstraintSet.START,
                        textView[i - 1].getId(),
                        ConstraintSet.END,
                        10);
            }


        }
        //Aplicamos los constraint
        constraintSet.applyTo(layout);


        return textView;
    }
    /*
        Metodo que comprueba si la palabra2 se puede generar con las letras de la palabra1
        */
    boolean esParaulaSolucio(String palabra1, String palabra2) {
        char[] palabra3 = palabra2.toCharArray();
        //Crearemos este hasMap ya que la palabra1 siempre sera la de letras disponibles
        HashMap<Character, Integer> nada = new HashMap<Character, Integer>(letrasdisponibles);

        //Pasamos por todas las letras de palabra2 sino la tiene palabra1 entonces devolvemos false
        for (int j = 0; j < palabra3.length; j++) {

            if (nada.containsKey(palabra3[j])) {
                //Si al restarle 1 a las veces que sale esa letra es 0 la quitamos
                Integer i = nada.get(palabra3[j]);
                nada.put(palabra3[j], i - 1);
                if (nada.get(palabra3[j]) == 0) {
                    nada.remove(palabra3[j]);
                }
            } else {
                return false;
            }

        }
//Si llegamos aqui quiere decir que tiene todas la letras para generar palabra2
        return true;


    }
    /*
        Metodo que muestra la palabra en el array de textView asignado
        */
    private void mostraParaula(String s, int posicio) {

        Character dato;
        for (int i = 0; i < palabras[posicio].length; i++) {
            palabras[posicio][i].setTextSize(35);
            dato = s.charAt(i);
            palabras[posicio][i].setText(dato.toString());
        }


    }
    /*
	Metodo que muestra la primera letra de una palabra oculta
	*/

    private void mostraPrimeraLletra(String s, int posicio) {


        palabras[posicio-1][0].setTextSize(35);
        Character c= s.charAt(0);
        palabras[posicio-1][0].setText(c.toString());

    }
    /*
	Muestra una notificacion con el mesaje que pasamos
	*/
    private void mostraMissatge(String s, boolean llarg) {
        Context context = getApplicationContext();
        CharSequence text = s;

        int duration;
        if (llarg) {
            duration = Toast.LENGTH_LONG;
        } else {
            duration = Toast.LENGTH_SHORT;
        }

            Toast toast = Toast.makeText(context, text, duration);
            toast.show();



    }
    /*
	Metodo que se llama al reiniciar una partida para crear una nueva wordletgh, elegir una palabra de esa misma longitud,
	llenar el hasMap de letrasdisponibles, además de selecionar todas las palabras soluciones posibles, para luego
	llamar al metodo que elegira todas las palbras ocultas
	*/
    private void inicializar() {
        Random x = new Random();
        wordLenght = x.nextInt((7 - 3) + 1) + 3;

          palabra_solucion = new HashMap<Integer, HashSet<String>>();
         palabra_oculta = new HashMap<Integer, String>();
          soluciones = new HashSet();

          encontradas = new TreeSet<String>();


        letrasdisponibles=new HashMap<Character,Integer>();
        HashSet<String> aux = palabras_longitud.get(wordLenght);

        List<String> list = new ArrayList<>(aux);
        // Generar un índice aleatorio
        int randomIndex = x.nextInt(list.size());

        // Recoger el elemento aleatorio
        String randomElement = list.get(randomIndex);
        for (char i : randomElement.toCharArray()) {
            Integer w2 = letrasdisponibles.get(i);
            if (w2 == null) {
                letrasdisponibles.put(i, 1);
            } else {
                letrasdisponibles.put(i, w2 + 1);
            }
        }

        for (int i = 3; i < 8; i++) {

            HashSet<String> nada = palabras_longitud.get(i);
            HashSet<String> punto = new HashSet<String>();
            Iterator iteradorPrueba = punto.iterator();
            Iterator itSeries = nada.iterator();
            while (itSeries.hasNext()) {

                String sol = itSeries.next().toString();


                //Comprabar si es una palabra solucion
                if (esParaulaSolucio(randomElement, sol)) {
                    //Meterla en soluciones y en el auxiliar punto para más tarde meterlo en palabras solucion
                    soluciones.add(sol);
                    punto.add(sol);
                }
            }



            palabra_solucion.put(i, punto);
        }

        elegir(randomElement);


    }
    /*
	Metodo en el que eligiremos las cinco palabras ocultas
	*/

    private void elegir(String random) {
        Random x = new Random();
        TreeSet<String> ordenadas = new TreeSet<String>();
        int j = 4;
        //Metemos en la ultima posicion la palabra aleatoria elegida en inicializar
        int sol = 0;
        HashSet<String> comprobar;
        int i = wordLenght - 1;


        palabra_oculta.put(5, random);


        //Meteremos de uno en uno las palabra de más de longitud 3
        for (; i > 3; i--) {
            comprobar = palabra_solucion.get(i);
            if (!comprobar.isEmpty()) {



                List<String> list = new ArrayList<>(palabra_solucion.get(i));

                int randomIndex = x.nextInt(list.size());

                String randomElement = list.get(randomIndex);
                if(!random.equals(randomElement)) {
                    palabra_oculta.put(j, randomElement);
                    j--;
                }
            }
        }
        comprobar = new HashSet<String>(palabra_solucion.get(3));
        int k=j;
        //PUEDE DAR EL CASO QUE HAYA MENOS DE 5 PLABRAS SOLUCION YA QUE NO HAY SIFICIENTES PALABRAS DE LONGITUD 3
        //Meteremos todas las posibles palabras de longitud 3 que sean solucion
            for (; j > 0; j--) {

                if (!comprobar.isEmpty()) {
                    List<String> list = new ArrayList<>(comprobar);

                    int randomIndex = x.nextInt(list.size());

                    // Recoger el elemento aleatorio
                    String randomElement = list.get(randomIndex);
                    if(!random.equals(randomElement)) {
                        ordenadas.add(randomElement);
                        comprobar.remove(randomElement);
                        k--;
                    }
                }

            }
        //Lo meteremos en palabra solucion
            Iterator it = ordenadas.iterator();
            int segir = 1+k;
            while (it.hasNext()) {
                palabra_oculta.put(segir, it.next().toString());
                segir++;
            }




    }
    /*
	Metodo que llamara a iniciartodo para empezar una nueva partida además de deshabilitar todos los view
	*/
    public void reiniciar(View view) {

        iniciarTodo();
        enableViews(R.id.background);


    }
    /*
	Metodo que empieza una nueva partida, para inicalizar todo de nuevo
	*/
    public void iniciarTodo() {

        for (int i=0;i<botones.length;i++){
            botones[i].setEnabled(true);
            botones[i].setVisibility(View.VISIBLE);
        }
        ConstraintLayout background =  findViewById(R.id.background);
        //Vaciamos todos los arrays de TextView
        for (int i = 0; i < 5; i++) {

            if(palabras[i]!=null) {
                for (int j = 0; j < palabras[i].length; j++) {
                    background.removeView(palabras[i][j]);
                }
            }
        }
        //Llamamos al metodo inicializar

        inicializar();
        //Metodo que asigna las letrasdisponibles a los botenes que estan en el circulo
        int j = 0;
        Iterator<Map.Entry<Character, Integer>> it = letrasdisponibles.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<Character, Integer> entry = (Map.Entry<Character, Integer>) it.next();
            Character letra = entry.getKey();
            String letras = letra.toString();
            int num = entry.getValue();
            botones[j].setText(letras);
            if (num > 1) {
                //Si aparace más veces tenemos que meterla en un boton siguiente
                num--;
                while (num >= 1) {
                    j++;
                    botones[j].setText(letras);
                    num--;

                }
            }
            j++;
        }
        //Inicializamos valores para inicar la partida
        encertado = findViewById(R.id.encertado);
        posibles = tamaño(soluciones);
        acertadas1= new TreeSet<>();
        acertadas=0;
        acertadasbonus=0;
        printEncertado = "Has encertat " + acertadas + " de " + posibles + " possibles:";
        encertado.setText(printEncertado);
        for (; j < botones.length; j++) {
            View dato = botones[j];
            dato.setEnabled(false);
            dato.setVisibility(View.GONE);
        }

        //Creamos todos los arrays de TextView posibles
        if(palabra_oculta.containsKey(1)) {
            palabra1 = crearFilaTextViews(R.id.g1, palabra_oculta.get(1).length());
        }
        if(palabra_oculta.containsKey(2)) {
            palabra2 = crearFilaTextViews(R.id.g2, palabra_oculta.get(2).length());
        }
        if(palabra_oculta.containsKey(3)) {
            palabra3 = crearFilaTextViews(R.id.g3, palabra_oculta.get(3).length());
        }
        if(palabra_oculta.containsKey(4)) {
            palabra4 = crearFilaTextViews(R.id.g4, palabra_oculta.get(4).length());
        }
        palabra5 = crearFilaTextViews(R.id.g5, palabra_oculta.get(5).length());
        for (int i = 0; i < 5; i++) {
            switch (i) {
                case 0:
                    palabras[i] = palabra1;
                    break;
                case 1:
                    palabras[i] = palabra2;
                    break;

                case 2:
                    palabras[i] = palabra3;
                    break;
                case 3:
                    palabras[i] = palabra4;
                    break;
                case 4:
                    palabras[i] = palabra5;
                    break;

            }
        }
        View circulo = findViewById(R.id.circulo);
        Random r = new Random();
        int color = colores[r.nextInt(5)];

        int generador = r.nextInt();

        //Cambiamos el color de los arrays de textView
        circulo.setBackgroundColor(color);
        if(palabras[0]!=null) {
            cambiarcolor(palabras[0], palabras[0].length, color);
        }
        if(palabras[1]!=null) {
            cambiarcolor(palabras[1], palabras[1].length, color);
        }
        if(palabras[2]!=null) {
            cambiarcolor(palabras[2], palabras[2].length, color);
        }
        if(palabras[3]!=null) {
            cambiarcolor(palabras[3], palabras[3].length, color);
        }
        cambiarcolor(palabras[4], palabras[4].length, color);

        Iterator<Map.Entry<Integer, String>> it4 = palabra_oculta.entrySet().iterator();
        while (it4.hasNext()) {
            Map.Entry<Integer, String> entry = (Map.Entry<Integer, String>) it4.next();

            String value = entry.getValue();
            System.out.println("PALABRA SOLUCION: " + value);

        }
        Iterator it2 = soluciones.iterator();
        while (it2.hasNext()) {


            String value = it2.next().toString();
            System.out.println("SOY UNA POSIBLE SOLUCION: " + value);

        }
        Iterator<Map.Entry<Character, Integer>> it5 = letrasdisponibles.entrySet().iterator();
        while (it5.hasNext()) {
            Map.Entry<Character, Integer> entry = (Map.Entry<Character, Integer>) it5.next();

            Character value = entry.getKey();
            System.out.println("LETRA DISPONIBLE: " + value.toString());

        }
    }

    //Metodo que cambia el color del backgroud a un array de TextView
    private void cambiarcolor(TextView[] dato, int length, int color) {

        for (int i = 0; i < length; i++) {
            dato[i].setBackgroundColor(color);
        }
    }
    //Metodo que habilita todos los views a traves del id del Contraint layout
    private void enableViews(int parent) {
        //encontrar el id, mediante el padre
        ConstraintLayout dato = (ConstraintLayout) findViewById(parent);
        ViewGroup grupo = (ViewGroup) dato;
        int hijos = grupo.getChildCount();
        View dato1;

        for (int i = 0; i <= hijos; i++) {

            dato1 = grupo.getChildAt(i);
            if (dato1 == null) {
                break;
            }
            dato1.setEnabled(true);

        }

    }
    //Metodo que deshabilita todos los views, excepto bonus y reiniciar mediante
    //el id del layout que tenemos en el background
    private void disableViews(int parent) {

        ConstraintLayout background = (ConstraintLayout) findViewById(parent);
        ViewGroup grupo = (ViewGroup) background;
        int hijos = grupo.getChildCount();
        View dato1;

        for (int i = 0; i <= hijos; i++) {
            dato1 = grupo.getChildAt(i);
            if (dato1 == null) {
                break;
            }
            if ((dato1.getId() == R.id.bonus) || (dato1.getId() == R.id.reset)) {
                continue;
            }
            dato1.setEnabled(false);

        }

    }


    /*
Metodo que al enviar una palabra comprueba si es una palabra oculta o una palabra posibles y
la visualiza dependiendo si es una oculta o posible, además de actualizar el numero de palabras acertidas
*/
    public void EnviarPalabra(View view) {
        boolean essolucion=false;

        for (int i = 0; i < wordLenght; i++) {

            botones[i].setEnabled(true);

        }
            TextView palabraEncriptada = findViewById(R.id.PalabraEnc);
            String texto = palabraEncriptada.getText().toString();
            palabraEncriptada.setText("");
            String palabraAPoner;
            palabrasavisualizar="";
            int key = 0;
        //Comprobamos si es una de las 5 palabras ocultas si la es la buscamos
            if (palabra_oculta.containsValue(texto)) {

                essolucion=true;
                Iterator<Map.Entry<Integer, String>> it = palabra_oculta.entrySet().iterator();
                while (it.hasNext()) {
                    Map.Entry<Integer, String> entry = (Map.Entry<Integer, String>) it.next();
                    key = entry.getKey();
                    String value = entry.getValue();
                    if (texto.compareTo(value) == 0) {
                        break;
                    }

                }

                palabraAPoner = palabras_validas.get(texto);
                //La visualizamos en el array de textView y tambien en el texto de arriba
                mostraParaula(palabraAPoner, key - 1);
                acertadas++;
                acertadasbonus++;

                acertadas1.add(palabraAPoner);
                Iterator it4 = acertadas1.iterator();
                while (it4.hasNext()) {
                    palabrasavisualizar = palabrasavisualizar + "," + it4.next().toString();
                }
                printEncertado = "Has encertat " + acertadas + " de " + posibles + " possibles:" + palabrasavisualizar;

                encertado.setText(printEncertado);
                soluciones.remove(palabraAPoner);
                palabra_oculta.remove(key);
                mostraMissatge("Has encertat una paraula oculta, el teu bonus es de: "+acertadasbonus,true);

            } else if (soluciones.contains(texto)){
                //Si es solo una solucion la visualizamos en el texto de arriba unicamente
                essolucion=true;
                    palabraAPoner = palabras_validas.get(texto);
                    acertadas1.add(palabraAPoner);
                    Iterator it4 = acertadas1.iterator();
                    while (it4.hasNext()) {
                        palabrasavisualizar = palabrasavisualizar + "," + it4.next().toString();
                    }
                    acertadas++;
                    acertadasbonus++;

                    printEncertado = "Has encertat " + acertadas + " de " + posibles + " possibles:" + palabrasavisualizar;
                    encertado.setText(printEncertado);
                    soluciones.remove(palabraAPoner);
                    mostraMissatge("Has encertat una paraula extra, el teu bonus es de: "+acertadasbonus,true);




            }
            if(essolucion==false){
                mostraMissatge("Paraula no vàlida",true);
            }


        if(palabra_oculta.isEmpty())

        {
            mostraMissatge("Enhorabona has guanyat",true);



            disableViews(R.id.background);

        }

        }


        private <E>int tamaño(Set<E> conjunto){
        int size=0;
        Iterator it= conjunto.iterator();
        while (it.hasNext()){
            it.next();
            size++;
        }
        return size;
        }

    }
