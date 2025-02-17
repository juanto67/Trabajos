/*
Entrega 2
Dylan Luigi Canning García
Juan Marí González
Antonio Marí González
Descrición:
1. Implementación Mini-Shell
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COMMAND_LINE_SIZE 1024
#define MAX_ARGS 64
#define COMMAND_LINE_SIZE 1024
#define MAX_ARGS 64

int check_internal(char **args);
int parse_args(char **args, char *line);
int execute_line(char *line);
char *read_line(char *line);
void imprimir_prompt();
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs();
int DEBUG_FLAGS[] = {0, 1};
/*
 * Función:  imprimir_prompt
 * -------------------
 * Imprime el directorio actual + '$'
 */
void imprimir_prompt()
{
    char cwd[COMMAND_LINE_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s $ ", cwd); // Print del dir. actual + $.
    }
    else
    {
        perror("getcwd() error");
    }
}

/*
 * Función:  read_line
 * -------------------
 * Función encargada de imprimir el prompt y leer una linea de
 * la consola con la función fgets. Cuando ya obtenemos esa línea y
 * comprobamos que tiene contenido, cambiaremos el salto de línea por NULL
 * (cosa que nos servirá para el execute line en un futuro)
 *
 * También usamos la función fflush, que fuerza el vaciado del buffer para que
 * el buffer expulse sus contenidos(por si tiene mensajes en cola) y así mostrar
 * el prompt cuanto antes.
 *
 *
 * line: puntero que apunta a la línea de la consola(stdin)
 *
 *
 * retorna: puntero que apunta a la línea pero en vez de tener salto de línea
 * tendra null.
 */
char *read_line(char *line)
{
    imprimir_prompt();
    fflush(stdout); // Vaciamos el buffer de salida.
    if (fgets(line, COMMAND_LINE_SIZE, stdin) == NULL)
    {
        if (feof(stdin))
        { // Comprueba si es EOF aka (Ctrl+D).
            printf("\nLogout\n");
            exit(0);
        }
        else
        {
            perror("fgets() error");
            exit(1);
        }
    }
    size_t length = strlen(line);
    if (line[length - 1] == '\n')
    {
        line[length - 1] = '\0'; // Sustituye el carácter de nueva línea por un carácter nulo.
    }
    return line;
}

/*
 * Función: execute_line
 * Función encargada de ejecutar la línea, en este nivel
 * solo instanciaremos el array, separaremos los argumentos
 * por tokens, mediante el parse_args y haremos comandos internos
 * mediante check_inernal
 *
 *
 * line : puntero que apunta a la línea que pasamos por consola(stdin)
 *
 *
 * retorna: valor dado por check_internal (si es un comando interno 1(-1 si hay un error) si no lo es 2)
 */
int execute_line(char *line)
{
    char *args[MAX_ARGS];
    int num_tokens = parse_args(args, line);

    if (num_tokens > 0)
    {
        return check_internal(args);
    }
    return 0;
}

/*
 * Función: parse_args
 * -------------------
 * Trocea la linea obtenida en tokens mediante la función strtok, en nuestro caso los tokens
 * estarán delimitados por los caracteres espacio, tab, salto de línea y return. Por otro lado,
 * tenemos en cuenta que el # representa un comentario, por tanto ignoramos los tokens posteriores a
 * este carácter.
 *
 *
 * args: array de arrays en el que introduciremos los tokens
 * line: linea introducida en consola (stdin)
 *
 * retorna: el número de tokens encontrados
 */
int parse_args(char **args, char *line)
{
    int num_tokens = 0;
    char *token = strtok(line, " \t\n\r");
    while (token != NULL && num_tokens < MAX_ARGS - 1)
    {
        if (token[0] == '#')
        {
            break;
        }

        args[num_tokens++] = token;
        token = strtok(NULL, " \t\n\r");
        if (DEBUG_FLAGS[0] == 1)
        {
            printf("parse_args()->El token número: %d es: %s \n", num_tokens, args[num_tokens - 1]);
        }
    }
    if (DEBUG_FLAGS[0] == 1)
    {
        printf("parse_args()-> el número de tokens es: %d  \n", num_tokens);
    }
    args[num_tokens] = NULL;

    return num_tokens;
}

/*
 * Función: check_internal
 * -------------------
 * Función encargada de detectar si el comando pasado es interno
 * simplemente comprueba si el primer argumento es igual al comando interno,
 * en el caso que lo sea realizaremos dicha función. En el caso de que el comando
 * no sea ninguna función interna se devolverá 0.
 *
 *
 * args: array de arrays con los tokens
 *
 *
 * retorna: devuelve 1 en el caso de que sea interna y vaya bien, -1 si hay un error dentro de la instrucción y 2 en el caso de que no lo sea.
 */
int check_internal(char **args)
{
    if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        if (strcmp(args[0], "cd") == 0)
        {
            return internal_cd(args);
        }
        else if (strcmp(args[0], "export") == 0)
        {
            return internal_export(args);
        }
        else if (strcmp(args[0], "source") == 0)
        {
            return internal_source(args);
        }
        else if (strcmp(args[0], "jobs") == 0)
        {
            return internal_jobs();
        }
        return 2;
    }
}

/*
 * Función:  internal_cd
 *
 * Esta función será realizará el comando cd, dicho comando nos cambiará el directorio de trabajo.
 * Lo primero que realizaremos es dos arrays con dimension COMMAND_LINE_SIZE. El primero lo utilizaremos
 * para comprobar si hemos cambiado de directorio. En el segundo, almacenaremos el directorio indicado
 * según los casos. Los casos son(hay que recalcar que cambiamos de directorio mediante la función chdir
 * en todos):
 *
 * ---- CD para volver al directorio home--------
 *
 *  Este cd no tiene un segundo argumento, es decir el comando sería así: cd NULL,
 *  para este caso simplemente detectamos que no hay segundo argumento y almacenamos
 * en la variable dir el directorio home, mediante la función getenv que nos devuelve
 * la variable de entorno indicada en el parámetro. Posteriormente comprobamos
 * si dir tiene algún contenido, y cambiamos de directorio(se hace en la línea 232)
 *
 * -------- CD para un directorio específico---------
 * Entraremos en un bucle en el que comprobaremos si hay comillas en los argumentos,
 * saldremos de dicho bucle sin ningún cambio y el if de la \ no se cumplirá, así que
 * no tocaremos los argumentos y haremos chdir al final.
 *
 *
 *---------- CD de caso avanzado----------------
 *
 * Este cd admite el paso de directorios con espacios mediante una escritura especial
 * en este caso manejaremos estos casos : cd 'mini shell', cd "mini shell", cd mini/ shell.
 *
 * Primero comprobaremos las comillas, haremos un bucle en el que pasaremos argumento por
 * argumento detectando si hay comillas al inicio o al final.
 *
 *
 * args: array de arrays en el que están los tokens
 *
 * retorna: devuelve 1, -1 si hay error
 */
int internal_cd(char **args)
{
    char cwd[COMMAND_LINE_SIZE];
    char dir[COMMAND_LINE_SIZE] = {0};

    if (args[1] == NULL)
    {
        // Sin argumentos, ir al directorio HOME
        strcpy(dir, getenv("HOME"));
    }
    else
    {
        // Manejar argumentos con comillas y posibles espacios escapados
        int i = 1;
        while (args[i] != NULL)
        {
            char *arg = args[i];
            int inQuote = 0;
            char quoteChar = '\0';

            while (*arg)
            {
                if ((*arg == '\'' || *arg == '\"') && !inQuote)
                {
                    // Inicio de una cadena entrecomillada
                    inQuote = 1;
                    quoteChar = *arg;
                }
                else if (*arg == quoteChar && inQuote)
                {
                    // Fin de una cadena entrecomillada
                    inQuote = 0;
                }
                else
                {
                    // Agregar el carácter si no es comilla de inicio o fin
                    if (!inQuote || (*arg != quoteChar))
                    {
                        size_t len = strlen(dir);
                        dir[len] = *arg;
                        dir[len + 1] = '\0';
                    }
                }
                arg++;
            }

            // Si el último carácter del argumento actual es una barra invertida
            // y el siguiente argumento no es NULL (caso de espacio escapado)
            if (args[i][strlen(args[i]) - 1] == '\\' && args[i + 1] != NULL)
            {
                // Reemplazar la barra invertida por un espacio
                dir[strlen(dir) - 1] = ' ';
            }
            else if (args[i + 1] != NULL)
            {
                // Agregar un espacio si el siguiente argumento no es NULL
                strcat(dir, " ");
            }
            i++;
        }
    }

    // Eliminar el espacio final si existe
    if (dir[strlen(dir) - 1] == ' ')
    {
        dir[strlen(dir) - 1] = '\0';
    }

    // Cambiar de directorio
    if (chdir(dir) != 0)
    {
        perror("Error en chdir()");
        return -1;
    }

    // Obtener e imprimir el directorio actual de trabajo
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Cambiado al directorio: %s\n", cwd);
    }
    else
    {
        perror("Error en getcwd()");
        return -1;
    }

    return 1;
}

/*
 * Función:  internal_export
 *
 * Esta función lo que hace es modificar una variable de entorno con el valor pasado
 * por parámetro, el formato sería así: export HOME=hola.
 *
 * En el caso de que no haya segundo argumento, significará que hay un error de sintaxis
 * por tanto lo notificaremos al usuario.
 *
 * Si tiene segundo argumento, lo que haremos es:
 * guardar el nombre en una variable y mediante la función strchar, tener un puntero que apunte
 * al = , lo que significará que los siguientes caracteres serán el valor pasado. Posteriormente,
 * detectaremos otro error de sintaxis, en el cual nombre es igual al valor dado por el usuario.
 *
 * Por otro lado, si no hay errores, pondremos que el = será /0(para facilitar la parte del nombre) y
 * nos moveremos al siguiente caracter (el primero del valor). Mediante la función getenv conseguiremos
 * la variable pedida y mediante setenv cambiaremos su contenido.
 *
 *
 * args = array de arrays en el que tenemos todos los tokens
 *
 *
 * retorna:-1 si da error, 1 si funciona correctamente.
 */
int internal_export(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "Error de sintaxis. Uso: export Nombre=Valor\n");
        return -1;
    }

    char *name = args[1];
    char *value = strchr(args[1], '=');

    if (value == NULL || value == name)
    {
        fprintf(stderr, "Error de sintaxis. Uso: export Nombre=Valor\n");
        return -1;
    }

    *value = '\0';
    value++;

    char *old_value = getenv(name);
    if (DEBUG_FLAGS[1] == 1)
    {
        printf("[internal_export()→ antiguo valor para %s: %s]\n", name, old_value ? old_value : "(null)");
    }

    if (setenv(name, value, 1) != 0)
    {
        perror("setenv() error");
        return -1;
    }

    char *new_value = getenv(name);
    if (DEBUG_FLAGS[1] == 1)
    {
        printf("[internal_export()→ nuevo valor para %s: %s]\n", name, new_value ? new_value : "(null)");
    }
    return 1;
}

/*
 * Función:
 * -------------------
 *
 *
 * dest:
 * src:
 *
 * retorna:
 */
int internal_source(char **args)
{
    printf("internal_source()->Ejecuta las líneas de comandos del fichero nombre_fichero \n");
    return 1;
}

/*
 * Función:
 * -------------------
 *
 *
 * dest:
 * src:
 *
 * retorna:
 */
int internal_jobs()
{
    printf("internal_jobs()->Muestra el PID, el estado y el nombre de los procesos que se están ejecutando en background o que han sido detenidos\n");

    return 1;
}

/*
 * Función:
 * -------------------
 *
 *
 * dest:
 * src:
 *
 * retorna:
 */
int internal_fg(char **args)
{
    printf("internal_fg()->Envía un trabajo del background al foreground, o reactiva la ejecución en foreground de un trabajo que había sido detenido.\n");

    return 1;
}

/*
 * Función:
 * -------------------
 *
 *
 * dest:
 * src:
 *
 * retorna:
 */
int internal_bg(char **args)
{
    printf("internal_bg->Reactiva un proceso detenido para que siga ejecutándose pero en segundo plano.\n");

    return 1;
}

/*
 * Función:  main
 * -------------------
 *  El main de este nivel únicamente contiene el bucle infinito
 *  que realizará el funcionamiento básico del mini shell. Simplemente
 *  leerá las líneas(read_line) y posteriormente las ejecutará(execute_line).
 *
 *
 *
 *
 * retorna: Siempre devuelve 0.
 */
int main()
{
    char line[COMMAND_LINE_SIZE];
    while (1)
    {
        if (read_line(line))
        {
            execute_line(line);
        }
    }
    return 0;
}
