/*
    Grupo: EDITOR

    Apellido(s), nombre(s): Castillo, Martin
    DNI: 46196957
    Entrega: Sí

    Apellido(s), nombre(s): Tamburrini, Nicolas
    DNI: 46573624
    Entrega: Sí

    Apellido(s), nombre(s): Aguila, Mateo Ezequiel
    DNI: 46563915
    Entrega: Sí

    Apellido(s), nombre(s): Saquilan, Tomas
    DNI: 42283011
    Entrega: Sí
*/

// [APRENDER] ===================================================================
// [APRENDER] COMO SE CONECTAN LOS ARCHIVOS (mapa de este proyecto)
// [APRENDER] ===================================================================
// [APRENDER]   main.c                llama a solucion()
// [APRENDER]   funciones_grupo.c     (ESTE archivo) define solucion(): el "director"
// [APRENDER]   funciones_castillo.c  filtros tuyos (Negativo, ...). Su .h los declara.
// [APRENDER]   compartido.c          leer/escribir BMP, usado por todos
// [APRENDER]
// [APRENDER] Cadena de #include que hace que ESTE archivo "vea" a Negativo y a sImagen:
// [APRENDER]   funciones_grupo.c -> funciones_grupo.h -> funciones_castillo.h -> compartido.h
// [APRENDER] Un #include pega el texto del .h en ese lugar.

#include <stdio.h>      // [APRENDER] printf, snprintf, NULL
#include <stdlib.h>     // [APRENDER] strtol (convierte texto a numero)
#include <string.h>     // [APRENDER] strlen, strcmp, strchr, strrchr, memcpy
#include <ctype.h>      // [APRENDER] tolower
#include "funciones_grupo.h"
#include "compartido.h"

// [APRENDER] Cada vez que aparezca NOMBRE_GRUPO el compilador pone "EDITOR".
#define NOMBRE_GRUPO "EDITOR"


// [APRENDER] ===================================================================
// [APRENDER] ETAPA 1: TIPO PUNTERO A FUNCION + TABLA DE FILTROS
// [APRENDER] ===================================================================
// [APRENDER] Hay DOS tipos de filtro segun lleven o no un porcentaje, y cada uno
// [APRENDER] tiene su propia firma. Asi un filtro que no usa el porcentaje no tiene
// [APRENDER] que recibirlo ni "silenciarlo" con (void)parametro.
// [APRENDER]
// [APRENDER] Como se lee:  sImagen* (*tFuncionSimple)(const sImagen *original)
// [APRENDER]   "tFuncionSimple es un PUNTERO a una funcion que recibe una imagen y
// [APRENDER]    devuelve una imagen". Ej: Negativo, EscalaDeGrises, espejar, rotar.
typedef sImagen* (*tFuncionSimple)(const sImagen *original);

// [APRENDER] "tFuncionPorcentaje es un PUNTERO a una funcion que recibe una imagen y
// [APRENDER] un int (el N de =N) y devuelve una imagen". Ej: contraste, tonalidades,
// [APRENDER] recortar, achicar.
typedef sImagen* (*tFuncionPorcentaje)(const sImagen *original, int porcentaje);

typedef struct
{
    const char         *nombre;             // [APRENDER] lo que escribe el usuario: "negativo"

    // [APRENDER] Cada filtro usa UNO de estos dos punteros; el otro queda en NULL.
    // [APRENDER] NULL tambien significa "todavia no esta escrito".
    tFuncionSimple      funcionSimple;      // [APRENDER] para filtros SIN =N
    tFuncionPorcentaje  funcionPorcentaje;  // [APRENDER] para filtros CON =N

    int                 pideParametro;      // [APRENDER] 1 = hay que escribir =N (elige cual puntero se usa)
    int                 minimo;             // [APRENDER] menor valor aceptado (1 en recortar/achicar)
} tFiltro;

// [APRENDER] Para "conectar" un filtro nuevo se pone el nombre de su funcion en la
// [APRENDER] columna que corresponde: SIN porcentaje en la 1ra, CON porcentaje en la 2da.
// [APRENDER] (su .h ya esta incluido en funciones_grupo.h).
// [APRENDER] Columnas:  { nombre, funcionSimple, funcionPorcentaje, pideParametro, minimo }
static const tFiltro TABLA_FILTROS[] =
{
    { "negativo",              Negativo,       NULL,           0, 0 },
    { "escala-de-grises",      EscalaDeGrises, NULL,           0, 0 },
    { "espejar-horizontal",    NULL,           NULL,           0, 0 },
    { "espejar-vertical",      NULL,           NULL,           0, 0 },
    { "aumentar-contraste",    NULL,           NULL,           1, 0 },
    { "reducir-contraste",     NULL,           NULL,           1, 0 },
    { "tonalidad-azul",        NULL,           TonalidadAzul,  1, 0 },
    { "tonalidad-verde",       NULL,           TonalidadVerde,           1, 0 },
    { "tonalidad-roja",        NULL,           TonalidadRojo,           1, 0 },
    { "recortar",              NULL,           NULL,           1, 1 },
    { "achicar",               NULL,           NULL,           1, 1 },
    { "rotar-derecha",         NULL,           NULL,           0, 0 },
    { "rotar-izquierda",       NULL,           NULL,           0, 0 },
    { "concatenar-horizontal", NULL,           NULL,           0, 0 },
    { "concatenar-vertical",   NULL,           NULL,           0, 0 },
    { "comodin",               NULL,           NULL,           0, 0 }
};

// [APRENDER] Cuantas filas tiene la tabla (tamanio total / tamanio de una fila).
// [APRENDER] Es una constante conocida al compilar, asi que se puede usar como
// [APRENDER] tamanio de un arreglo comun (estatico), sin malloc.
#define CANT_FILTROS ((int)(sizeof(TABLA_FILTROS) / sizeof(TABLA_FILTROS[0])))


// [APRENDER] ===================================================================
// [APRENDER] ETAPA 2: CODIGOS DE RESULTADO AL INTERPRETAR UN ARGUMENTO
// [APRENDER] ===================================================================
#define ARG_OK            0   // [APRENDER] todo bien
#define ARG_DESCONOCIDO   1   // [APRENDER] no esta en la tabla
#define ARG_FALTA_VALOR   2   // [APRENDER] pide =N y no lo trae
#define ARG_SOBRA_VALOR   3   // [APRENDER] trae =N y no lo admite
#define ARG_NO_NUMERO     4   // [APRENDER] el valor no es un entero (=abc, =1.5)
#define ARG_FUERA_RANGO   5   // [APRENDER] entero pero fuera de [minimo, 100]

#define LARGO_NOMBRE 64       // [APRENDER] buffer para copiar el nombre de un filtro
#define LARGO_RUTA   512      // [APRENDER] buffer para el nombre del archivo de salida


// [APRENDER] SinGuiones: "--negativo" -> "negativo". Devuelve un puntero a una
// [APRENDER] posicion DENTRO del mismo string ("arg + 2" avanza 2 caracteres).
static const char* SinGuiones(const char *arg)
{
    if(arg[0] == '-' && arg[1] == '-')
        return arg + 2;

    return arg;
}

// [APRENDER] BuscarFiltro: posicion en la tabla del filtro con ese nombre, o -1.
// [APRENDER] strcmp(a, b) devuelve 0 si los strings son iguales.
static int BuscarFiltro(const char *nombre)
{
    for(int i = 0; i < CANT_FILTROS; i++)
    {
        if(strcmp(nombre, TABLA_FILTROS[i].nombre) == 0)
            return i;
    }

    return -1;
}

// [APRENDER] EsArchivoBmp: 1 si el argumento termina en ".bmp" (cualquier mayuscula).
static int EsArchivoBmp(const char *arg)
{
    size_t largo = strlen(arg);
    if(largo < 4)
        return 0;

    const char *ext = arg + largo - 4;

    return ext[0] == '.'
        && tolower((unsigned char)ext[1]) == 'b'
        && tolower((unsigned char)ext[2]) == 'm'
        && tolower((unsigned char)ext[3]) == 'p';
}

// [APRENDER] SepararNombre: "aumentar-contraste=18" -> nombre="aumentar-contraste" y
// [APRENDER] devuelve un puntero a "18" (o NULL si no habia '='). strchr busca el '='
// [APRENDER] y (igual - arg) es la cantidad de caracteres antes de el.
// [APRENDER] Todo string termina en '\0'; al copiar a mano hay que ponerlo.
static const char* SepararNombre(const char *arg, char *nombre, size_t tam)
{
    const char *igual = strchr(arg, '=');
    size_t largo = (igual != NULL) ? (size_t)(igual - arg) : strlen(arg);

    if(largo >= tam)
        largo = tam - 1;

    memcpy(nombre, arg, largo);
    nombre[largo] = '\0';

    return (igual != NULL) ? igual + 1 : NULL;
}

// [APRENDER] ConvertirEntero: "18" (texto) -> 18 (numero). strtol deja en 'fin' un
// [APRENDER] puntero al primer caracter que NO pudo convertir; por eso se le pasa &fin
// [APRENDER] (puntero a puntero). Si 'fin' llego al '\0', todo el texto era un entero.
static int ConvertirEntero(const char *texto, long *numero)
{
    if(texto[0] == '\0')
        return 0;

    char *fin;
    *numero = strtol(texto, &fin, 10);

    return *fin == '\0';
}

// [APRENDER] InterpretarArgumento: junta todo lo anterior y devuelve un codigo ARG_*.
// [APRENDER] *posFiltro = fila de la tabla (si se encontro). *valor = N (si ARG_OK).
static int InterpretarArgumento(const char *arg, int *posFiltro, int *valor)
{
    char nombre[LARGO_NOMBRE];

    const char *sinGuiones = SinGuiones(arg);
    const char *textoValor = SepararNombre(sinGuiones, nombre, sizeof(nombre));

    int pos = BuscarFiltro(nombre);
    if(pos < 0)
        return ARG_DESCONOCIDO;

    *posFiltro = pos;

    const tFiltro *filtro = &TABLA_FILTROS[pos];

    if(!filtro->pideParametro)
    {
        if(textoValor != NULL)
            return ARG_SOBRA_VALOR;

        *valor = 0;
        return ARG_OK;
    }

    if(textoValor == NULL)
        return ARG_FALTA_VALOR;

    long numero;
    if(!ConvertirEntero(textoValor, &numero))
        return ARG_NO_NUMERO;

    if(numero < filtro->minimo || numero > 100)
        return ARG_FUERA_RANGO;

    *valor = (int)numero;
    return ARG_OK;
}

// [APRENDER] InformarError: solo muestra el mensaje; NO corta el programa, asi el
// [APRENDER] recorrido sigue con los demas argumentos (lo pide el enunciado).
static void InformarError(int codigo, const char *arg, int posFiltro)
{
    switch(codigo)
    {
    case ARG_DESCONOCIDO:
        printf("\nERROR: \"%s\" no es un filtro conocido. Se ignora.", arg);
        break;

    case ARG_FALTA_VALOR:
        printf("\nERROR: \"%s\" necesita un valor, por ejemplo --%s=10. Se ignora.",
               arg, TABLA_FILTROS[posFiltro].nombre);
        break;

    case ARG_SOBRA_VALOR:
        printf("\nERROR: \"%s\" no admite un valor (se escribe sin =N). Se ignora.", arg);
        break;

    case ARG_NO_NUMERO:
        printf("\nERROR: \"%s\": el valor no es un numero entero. Se ignora.", arg);
        break;

    case ARG_FUERA_RANGO:
        printf("\nERROR: \"%s\": el valor debe estar entre %d y 100. Se ignora.",
               arg, TABLA_FILTROS[posFiltro].minimo);
        break;

    default:
        break;
    }
}


// [APRENDER] ===================================================================
// [APRENDER] ETAPA 3, PIEZA 1: NOMBRE DEL ARCHIVO DE SALIDA
// [APRENDER] ===================================================================
// [APRENDER] El PDF pide: EDITOR_<filtro>_<imagen>.bmp, guardado en la carpeta desde
// [APRENDER] donde se ejecuta el programa. Si el usuario escribe una ruta como
// [APRENDER] "fotos\unlam.bmp", NO hay que copiar "fotos\": solo el nombre del archivo.

// [APRENDER] NombreBase: "C:\fotos\unlam.bmp" -> "unlam.bmp".
// [APRENDER] strrchr es como strchr pero busca desde el FINAL (la ULTIMA aparicion).
// [APRENDER] Windows usa '\' y a veces '/', asi que se buscan los dos y se queda con
// [APRENDER] el que este mas a la derecha (el de mayor direccion).
// [APRENDER] En C el caracter barra invertida se escribe '\\' (la primera \ es escape).
static const char* NombreBase(const char *ruta)
{
    const char *barra  = strrchr(ruta, '/');
    const char *barraI = strrchr(ruta, '\\');

    if(barraI != NULL && (barra == NULL || barraI > barra))
        barra = barraI;

    return (barra != NULL) ? barra + 1 : ruta;
}

// [APRENDER] ArmarNombreSalida: escribe "EDITOR_negativo_unlam.bmp" en 'destino'.
// [APRENDER] snprintf es printf pero que escribe en un buffer en vez de la pantalla, y
// [APRENDER] NUNCA escribe mas de 'tam' bytes (evita desbordar el buffer).
// [APRENDER] Devuelve cuantos caracteres HABRIA escrito: si es >= tam, no entro.
// [APRENDER] Devuelve 1 si entro bien, 0 si no.
static int ArmarNombreSalida(char *destino, size_t tam, const char *filtro, const char *rutaImagen)
{
    int escritos = snprintf(destino, tam, "%s_%s_%s", NOMBRE_GRUPO, filtro, NombreBase(rutaImagen));

    return escritos > 0 && (size_t)escritos < tam;
}


// [APRENDER] ===================================================================
// [APRENDER] ETAPA 3, PIEZA 2: EJECUTAR UN FILTRO Y GUARDAR
// [APRENDER] ===================================================================
// [APRENDER] Recibe la imagen ORIGINAL ya cargada y la fila de la tabla. Pasos:
// [APRENDER]   1) la funcion existe?        2) armar el nombre del archivo
// [APRENDER]   3) llamar al filtro          4) guardar con EscribirBMP
// [APRENDER]   5) liberar la imagen resultado
// [APRENDER] Devuelve 1 si se genero el archivo, 0 si no.
static int EjecutarFiltro(const sImagen *original, int pos, int valor, const char *rutaImagen)
{
    const tFiltro *filtro = &TABLA_FILTROS[pos];

    // [APRENDER] Cual de los dos punteros hay que mirar depende de si el filtro lleva
    // [APRENDER] =N. Si el que corresponde esta en NULL, nadie escribio todavia esa
    // [APRENDER] funcion: se avisa y se sigue.
    int implementado = filtro->pideParametro ? (filtro->funcionPorcentaje != NULL)
                                             : (filtro->funcionSimple != NULL);

    if(!implementado)
    {
        printf("\nAVISO: \"%s\" todavia no esta implementado. No se genera archivo.", filtro->nombre);
        return 0;
    }

    char salida[LARGO_RUTA];
    if(!ArmarNombreSalida(salida, sizeof(salida), filtro->nombre, rutaImagen))
    {
        printf("\nERROR: el nombre de salida de \"%s\" es demasiado largo.", filtro->nombre);
        return 0;
    }

    // [APRENDER] ESTA ES LA LLAMADA POR PUNTERO A FUNCION. No sabemos cual filtro es:
    // [APRENDER] llamamos a la funcion cuya direccion esta guardada en la tabla.
    // [APRENDER] Cada filtro devuelve una imagen NUEVA; la original no se toca, asi
    // [APRENDER] el proximo filtro vuelve a partir de la original.
    // [APRENDER] Con =N se le pasa el valor; sin =N, solo la imagen.
    sImagen *resultado;
    if(filtro->pideParametro)
        resultado = filtro->funcionPorcentaje(original, valor);
    else
        resultado = filtro->funcionSimple(original);

    if(resultado == NULL)
    {
        printf("\nERROR: el filtro \"%s\" no pudo procesar la imagen.", filtro->nombre);
        return 0;
    }

    int ok = EscribirBMP(salida, resultado);

    // [APRENDER] Todo lo que se pide con malloc se libera: el resultado ya esta
    // [APRENDER] guardado en disco, no hace falta mantenerlo en memoria.
    LiberarImagen(resultado);

    return ok;
}

// [APRENDER] EsConcatenar: los dos concatenar necesitan DOS imagenes, asi que no
// [APRENDER] entran en la firma (imagen, int). Se reconocen por el prefijo del nombre.
// [APRENDER] strncmp compara solo los primeros N caracteres.
static int EsConcatenar(int pos)
{
    return strncmp(TABLA_FILTROS[pos].nombre, "concatenar", 10) == 0;
}


// [APRENDER] ===================================================================
// [APRENDER] ETAPA 3, PIEZA 3: solucion() = EL DIRECTOR
// [APRENDER] ===================================================================
// [APRENDER] Hace dos pasadas:
// [APRENDER]   PASADA 1: recorre argv y ANOTA que se pidio (imagenes, filtros, valores).
// [APRENDER]             Aca se descartan repetidos y se informan los errores.
// [APRENDER]   PASADA 2: carga la imagen UNA vez y ejecuta cada filtro anotado.
// [APRENDER] Separarlo asi permite validar todo antes de hacer trabajo pesado.
int solucion(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("\nUso: %s [--filtros...] archivo.bmp", argv[0]);
        return 1;
    }

    // [APRENDER] Imagenes recibidas (maximo 2: la segunda es para concatenar-*).
    const char *imagenes[2] = { NULL, NULL };
    int cantImagenes = 0;

    // [APRENDER] Arreglos "paralelos" a la tabla: la posicion k de cada uno habla del
    // [APRENDER] filtro k de TABLA_FILTROS.
    // [APRENDER]   pedido[k]  = 1 si ese filtro ya fue pedido  (sirve para los repetidos)
    // [APRENDER]   valores[k] = el N que se pidio
    // [APRENDER]   orden[]    = posiciones en el orden en que aparecieron en la linea
    // [APRENDER] "= {0}" pone todos los elementos en 0.
    int pedido[CANT_FILTROS]  = {0};
    int valores[CANT_FILTROS] = {0};
    int orden[CANT_FILTROS]   = {0};
    int cantPedidos = 0;

    // [APRENDER] Pasa a 1 si algo fallo, para devolver un codigo de error al final.
    int hayErrores = 0;

    // ------------------------------------------------------------ PASADA 1
    for(int i = 1; i < argc; i++)
    {
        if(EsArchivoBmp(argv[i]))
        {
            if(cantImagenes < 2)
            {
                imagenes[cantImagenes] = argv[i];
                cantImagenes++;
            }
            else
            {
                printf("\nAVISO: \"%s\": solo se usan las dos primeras imagenes. Se ignora.", argv[i]);
            }
            continue;
        }

        int pos = -1;
        int valor = 0;
        int resultado = InterpretarArgumento(argv[i], &pos, &valor);

        if(resultado != ARG_OK)
        {
            InformarError(resultado, argv[i], pos);
            hayErrores = 1;
            continue;
        }

        // [APRENDER] Repetido: "--negativo --negativo" se ejecuta UNA sola vez (lo
        // [APRENDER] pide el enunciado). Nos quedamos con la primera aparicion.
        if(pedido[pos])
        {
            printf("\nAVISO: \"%s\" esta repetido. Se ejecuta una sola vez.", argv[i]);
            continue;
        }

        pedido[pos]  = 1;
        valores[pos] = valor;
        orden[cantPedidos] = pos;
        cantPedidos++;
    }

    // [APRENDER] Sin imagen o sin filtros validos no hay nada que hacer.
    if(cantImagenes == 0)
    {
        printf("\nERROR: no se indico ninguna imagen .bmp.");
        return 1;
    }

    if(cantPedidos == 0)
    {
        printf("\nERROR: no hay ningun filtro valido para ejecutar.");
        return 1;
    }

    // ------------------------------------------------------------ PASADA 2
    // [APRENDER] Se carga UNA vez; todos los filtros parten de esta misma original.
    sImagen *original = CargarImagen(imagenes[0]);
    if(original == NULL)
    {
        printf("\nERROR: no se pudo cargar la imagen \"%s\".", imagenes[0]);
        return 1;
    }

    for(int k = 0; k < cantPedidos; k++)
    {
        int pos = orden[k];

        if(EsConcatenar(pos))
        {
            if(cantImagenes < 2)
            {
                printf("\nERROR: \"%s\" necesita DOS imagenes y se recibio una sola.",
                       TABLA_FILTROS[pos].nombre);
            }
            else
            {
                printf("\nAVISO: \"%s\" todavia no esta conectado (usa dos imagenes).",
                       TABLA_FILTROS[pos].nombre);
            }
            hayErrores = 1;
            continue;
        }

        if(!EjecutarFiltro(original, pos, valores[pos], imagenes[0]))
            hayErrores = 1;
    }

    // [APRENDER] La original se cargo con malloc (CargarImagen): se libera al final.
    LiberarImagen(original);

    printf("\n");
    return hayErrores ? 1 : 0;
}
