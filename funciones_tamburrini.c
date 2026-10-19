#include "funciones_tamburrini.h"

bool ejecutarAchicar(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;

    // Hacemos una copia para trabajar sobre ella
    copia = copiarImagen(imagen);
    if (copia == NULL) {
        return false;
    }

    if (!achicarMatriz(copia, porcentaje)) {
        liberarImagen(copia);
        return false;
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    generarBMP(destino, copia);

    liberarImagen(copia);
    return true;
}

bool achicarMatriz(Imagen *imagen, int porcentaje)
{
    int altoViejo = imagen->cabeceraImagen.alto;
    int anchoViejo = imagen->cabeceraImagen.ancho;
    if (altoViejo < 0) altoViejo = -altoViejo;

    // Calculamos las nuevas dimensiones reducidas según el porcentaje
    int nuevoAlto = (altoViejo * porcentaje) / 100;
    int nuevoAncho = (anchoViejo * porcentaje) / 100;

    if (nuevoAlto <= 0) nuevoAlto = 1;
    if (nuevoAncho <= 0) nuevoAncho = 1;

    // Reservamos una matriz temporal con las NUEVAS dimensiones chicas
    Pixel **nuevaMatriz = malloc(nuevoAlto * sizeof(Pixel *));
    if (nuevaMatriz == NULL) return false;

    for (int i = 0; i < nuevoAlto; i++) {
        nuevaMatriz[i] = malloc(nuevoAncho * sizeof(Pixel));
        if (nuevaMatriz[i] == NULL) {
            while (i > 0) free(nuevaMatriz[--i]);
            free(nuevaMatriz);
            return false;
        }
    }

    // Mapeamos los píxeles proporcionalmente para no perder contenido
    for (int fila = 0; fila < nuevoAlto; fila++) {
        for (int col = 0; col < nuevoAncho; col++) {

            // Calculamos a qué posición de la imagen original corresponde
            int filaVieja = (fila * altoViejo) / nuevoAlto;
            int colVieja = (col * anchoViejo) / nuevoAncho;

            // Evitamos desbordamientos por redondeo
            if (filaVieja >= altoViejo) filaVieja = altoViejo - 1;
            if (colVieja >= anchoViejo) colVieja = anchoViejo - 1;

            nuevaMatriz[fila][col] = imagen->pixeles[filaVieja][colVieja];
        }
    }

    // Liberamos la matriz VIEJA usando tu función de filas
    for (int i = 0; i < altoViejo; i++) {
        free(imagen->pixeles[i]);
    }
    free(imagen->pixeles);

    // Actualizamos el puntero y las dimensiones en la cabecera
    imagen->pixeles = nuevaMatriz;
    imagen->cabeceraImagen.ancho = nuevoAncho;
    imagen->cabeceraImagen.alto = nuevoAlto;

    return true;
}
bool ejecutarConcatenar(Imagen *imagen1, int argc, char *argv[], const char *nombrefiltro, char *nomarch)
{
    // 1. Buscar el segundo archivo BMP distinto al original
    char *segundoArchivo = NULL;
    for (int k = 1; k < argc; k++) {
        if (esArchivoBMP(argv[k]) && strcmp(argv[k], nomarch) != 0) {
            segundoArchivo = argv[k];
            break;
        }
    }

    if (segundoArchivo == NULL) {
        fprintf(stderr, "Error: El filtro %s requiere una segunda imagen BMP.\n", nombrefiltro);
        return false;
    }

    // 2. Cargar la segunda imagen
    Imagen *imagen2 = cargarBMP(segundoArchivo);
    if (imagen2 == NULL) {
        return false;
    }

    // 3. Determinar si es horizontal o vertical según el comando recibido
    bool esHorizontal = (strcmp(nombrefiltro, "--concatenar-horizontal") == 0);

    // 4. Aplicar la lógica de concatenación de matrices
    Imagen *resultado = concatenarMatrices(imagen1, imagen2, esHorizontal);
    if (resultado == NULL) {
        liberarImagen(imagen2);
        return false;
    }

    // 5. Generar el nombre limpio (saltando los "--") y guardar el archivo
    char destino[TAM_MAX];
    const char *nombreLimpio = nombrefiltro;
    if (strncmp(nombreLimpio, "--", 2) == 0) {
        nombreLimpio += 2;
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, (char*)nombreLimpio, nomarch);
    generarBMP(destino, resultado);

    // 6. Limpieza de memoria temporal
    liberarImagen(imagen2);
    liberarImagen(resultado);

    return true;
}
Imagen *concatenarMatrices(const Imagen *img1, const Imagen *img2, bool esHorizontal)
{
    int alto1 = (img1->cabeceraImagen.alto < 0) ? -img1->cabeceraImagen.alto : img1->cabeceraImagen.alto;
    int ancho1 = img1->cabeceraImagen.ancho;

    int alto2 = (img2->cabeceraImagen.alto < 0) ? -img2->cabeceraImagen.alto : img2->cabeceraImagen.alto;
    int ancho2 = img2->cabeceraImagen.ancho;

    int nuevoAlto, nuevoAncho;

    if (esHorizontal) {
        nuevoAlto = (alto1 > alto2) ? alto1 : alto2;
        nuevoAncho = ancho1 + ancho2;
    } else {
        nuevoAlto = alto1 + alto2;
        nuevoAncho = (ancho1 > ancho2) ? ancho1 : ancho2;
    }

    // Creamos una estructura Imagen para el resultado
    Imagen *resultado = malloc(sizeof(Imagen));
    if (resultado == NULL) return NULL;

    // Copiamos las cabeceras de la primera imagen como base
    resultado->cabeceraArchivo = img1->cabeceraArchivo;
    resultado->cabeceraImagen = img1->cabeceraImagen;

    // Actualizamos las nuevas dimensiones en la cabecera
    resultado->cabeceraImagen.ancho = nuevoAncho;
    resultado->cabeceraImagen.alto = nuevoAlto; // (O positivo/negativo según manejes)
    resultado->pixeles = NULL;

    // Reservamos la matriz para el resultado usando tu función
    if (!reservarMatriz(resultado)) {
        free(resultado);
        return NULL;
    }

    // Inicializamos toda la matriz resultado en negro (0,0,0) por si hay espacios de relleno
    int f, c;
    for (f = 0; f < nuevoAlto; f++) {
        for (c = 0; c < nuevoAncho; c++) {
            resultado->pixeles[f][c].rojo = 1;
            resultado->pixeles[f][c].verde = 130;
            resultado->pixeles[f][c].azul = 130;
        }
    }

    // -------------------------------------------------------------
    // COPIA Y ACOMODO DE PÍXELES
    // -------------------------------------------------------------
    if (esHorizontal) {
        // Pegamos la Imagen 1 a la izquierda
        for (f = 0; f < alto1; f++) {
            for (c = 0; c < ancho1; c++) {
                resultado->pixeles[f][c] = img1->pixeles[f][c];
            }
        }
        // Pegamos la Imagen 2 a la derecha de la primera
        for (f = 0; f < alto2; f++) {
            for (c = 0; c < ancho2; c++) {
                resultado->pixeles[f][ancho1 + c] = img2->pixeles[f][c];
            }
        }
    } else {
        // Pegamos la Imagen 1 arriba
        for (f = 0; f < alto1; f++) {
            for (c = 0; c < ancho1; c++) {
                resultado->pixeles[f][c] = img1->pixeles[f][c];
            }
        }
        // Pegamos la Imagen 2 abajo de la primera
        for (f = 0; f < alto2; f++) {
            for (c = 0; c < ancho2; c++) {
                resultado->pixeles[alto1 + f][c] = img2->pixeles[f][c];
            }
        }
    }

    return resultado;
}
bool ejecutarNegativo(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX]; // ¡Arreglo con tamaño, no puntero suelto!
    Imagen *copia;
    (void)porcentaje;

    copia = copiarImagen(imagen);

    if (copia == NULL) {
        return false;
    }

    negativo(copia);
    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    generarBMP(destino, copia);

    liberarImagen(copia);

    return true;
}
void negativo(Imagen *imagen)
{
    int fila;
    int columna;
    int alto;
    int ancho;

    ancho = imagen->cabeceraImagen.ancho;
    alto = imagen->cabeceraImagen.alto;

    if (alto < 0) {
        alto = -alto;
    }

    for (fila = 0; fila < alto; fila++) {

        for (columna = 0; columna < ancho; columna++) {

            imagen->pixeles[fila][columna].rojo =
                255 - imagen->pixeles[fila][columna].rojo;

            imagen->pixeles[fila][columna].verde =
                255 - imagen->pixeles[fila][columna].verde;

            imagen->pixeles[fila][columna].azul =
                255 - imagen->pixeles[fila][columna].azul;
        }
    }
}
bool ejecutarAumentarContraste(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;
    copia = copiarImagen(imagen);

    if (copia == NULL) {
        return false;
    }

    aumentar_contraste(copia,porcentaje);

    // Generamos el nombre y guardamos
    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    generarBMP(destino, copia);

    liberarImagen(copia);

    return true;
}
void aumentar_contraste(Imagen *imagen, int porcentaje)
{
    int fila, columna;
    int alto = imagen->cabeceraImagen.alto;
    int ancho = imagen->cabeceraImagen.ancho;
    int altoAbs = (alto < 0) ? -alto : alto;

    for (fila = 0; fila < altoAbs; fila++) {
        for (columna = 0; columna < ancho; columna++) {

            // Trabajamos todo con int: sumamos 100 al porcentaje, multiplicamos y dividimos por 100 al final
            int rojo = 128 + (((int)imagen->pixeles[fila][columna].rojo - 128) * (100 + porcentaje)) / 100;
            int verde = 128 + (((int)imagen->pixeles[fila][columna].verde - 128) * (100 + porcentaje)) / 100;
            int azul = 128 + (((int)imagen->pixeles[fila][columna].azul - 128) * (100 + porcentaje)) / 100;

            // Restricción (Clamping)
            if (rojo < 0) {rojo = 0;} if (rojo > 255) {rojo = 255;}
            if (verde < 0) {verde = 0;} if (verde > 255) {verde = 255;}
            if (azul < 0) {azul = 0;} if (azul > 255) {azul = 255;}

            imagen->pixeles[fila][columna].rojo = (unsigned char)rojo;
            imagen->pixeles[fila][columna].verde = (unsigned char)verde;
            imagen->pixeles[fila][columna].azul = (unsigned char)azul;
        }
    }
}
bool ejecutarRotarDerecha(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;
    (void)porcentaje;

    copia = copiarImagen(imagen);
    if (copia == NULL) {
        return false;
    }

    // 1. Ejecutamos la rotación sobre la copia
    if (!rotarMatrizDerecha(copia)) {
        liberarImagen(copia);
        return false;
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    generarBMP(destino, copia);

    liberarImagen(copia);
    return true;
}

bool rotarMatrizDerecha(Imagen *imagen)
{
    int altoViejo = imagen->cabeceraImagen.alto;
    int anchoViejo = imagen->cabeceraImagen.ancho;
    if (altoViejo < 0) altoViejo = -altoViejo;

    // Nuevas dimensiones invertidas
    int nuevoAlto = anchoViejo;
    int nuevoAncho = altoViejo;

    // 1. Reservamos una matriz temporal con las NUEVAS dimensiones
    Pixel **nuevaMatriz = malloc(nuevoAlto * sizeof(Pixel *));
    if (nuevaMatriz == NULL) return false;

    for (int i = 0; i < nuevoAlto; i++) {
        nuevaMatriz[i] = malloc(nuevoAncho * sizeof(Pixel));
        if (nuevaMatriz[i] == NULL) {
            // Si falla, limpiamos lo asignado hasta ahora
            while (i > 0) free(nuevaMatriz[--i]);
            free(nuevaMatriz);
            return false;
        }
    }

    // 2. Llenamos la nueva matriz mapeando desde la vieja
    for (int fila = 0; fila < nuevoAlto; fila++) {
        for (int col = 0; col < nuevoAncho; col++) {
            nuevaMatriz[fila][col] = imagen->pixeles[altoViejo - 1 - col][fila];
        }
    }

    // 3. Liberamos la matriz VIEJA usando tu función de liberación de filas
    for (int i = 0; i < altoViejo; i++) {
        free(imagen->pixeles[i]);
    }
    free(imagen->pixeles);

    // 4. Actualizamos el puntero y las dimensiones en la cabecera de la imagen
    imagen->pixeles = nuevaMatriz;
    imagen->cabeceraImagen.ancho = nuevoAncho;
    imagen->cabeceraImagen.alto = nuevoAlto; // (O negativo si manejas alto invertido, pero positivo va bien para generarBMP)

    return true;
}
bool ejecutarRotarIzquierda(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;
    (void)porcentaje;

    copia = copiarImagen(imagen);
    if (copia == NULL) {
        return false;
    }

    // 1. Ejecutamos la rotación sobre la copia
    if (!rotarMatrizIzquierda(copia)) {
        liberarImagen(copia);
        return false;
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    generarBMP(destino, copia);

    liberarImagen(copia);
    return true;
}

bool rotarMatrizIzquierda(Imagen *imagen)
{
    int altoViejo = imagen->cabeceraImagen.alto;
    int anchoViejo = imagen->cabeceraImagen.ancho;
    if (altoViejo < 0) altoViejo = -altoViejo;

    // Nuevas dimensiones invertidas
    int nuevoAlto = anchoViejo;
    int nuevoAncho = altoViejo;

    // 1. Reservamos una matriz temporal con las NUEVAS dimensiones
    Pixel **nuevaMatriz = malloc(nuevoAlto * sizeof(Pixel *));
    if (nuevaMatriz == NULL) return false;

    for (int i = 0; i < nuevoAlto; i++) {
        nuevaMatriz[i] = malloc(nuevoAncho * sizeof(Pixel));
        if (nuevaMatriz[i] == NULL) {
            // Si falla, limpiamos lo asignado hasta ahora
            while (i > 0) free(nuevaMatriz[--i]);
            free(nuevaMatriz);
            return false;
        }
    }

    // 2. Llenamos la nueva matriz mapeando desde la vieja
    for (int fila = 0; fila < nuevoAlto; fila++) {
        for (int col = 0; col < nuevoAncho; col++) {
            nuevaMatriz[fila][col] = imagen->pixeles[col][anchoViejo-1-fila];
        }
    }

    // 3. Liberamos la matriz VIEJA usando tu función de liberación de filas
    for (int i = 0; i < altoViejo; i++) {
        free(imagen->pixeles[i]);
    }
    free(imagen->pixeles);

    // 4. Actualizamos el puntero y las dimensiones en la cabecera de la imagen
    imagen->pixeles = nuevaMatriz;
    imagen->cabeceraImagen.ancho = nuevoAncho;
    imagen->cabeceraImagen.alto = nuevoAlto; // (O negativo si manejas alto invertido, pero positivo va bien para generarBMP)

    return true;
}
