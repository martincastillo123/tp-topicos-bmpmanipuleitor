#include "funciones_saquilan.h"

bool ejecutarReducirContraste(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;

    // Hacemos una copia para no modificar la imagen original
    copia = copiarImagen(imagen);
    if (copia == NULL) {
        fprintf(stderr, "No hay memoria suficiente para aplicar %s.\n", nombrefiltro);
        return false;
    }

    reducir_contraste(copia, porcentaje);

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    if (!generarBMP(destino, copia)) {
        fprintf(stderr, "No se pudo crear el archivo %s.\n", destino);
        liberarImagen(copia);
        return false;
    }

    liberarImagen(copia);
    return true;
}

void reducir_contraste(Imagen *imagen, int porcentaje)
{
    int fila, columna;
    int alto = imagen->cabeceraImagen.alto;
    int ancho = imagen->cabeceraImagen.ancho;
    int altoAbs = (alto < 0) ? -alto : alto;

    for (fila = 0; fila < altoAbs; fila++) {
        for (columna = 0; columna < ancho; columna++) {

            int rojo = 128 + (((int)imagen->pixeles[fila][columna].rojo - 128) * (100 - porcentaje)) / 100;
            int verde = 128 + (((int)imagen->pixeles[fila][columna].verde - 128) * (100 - porcentaje)) / 100;
            int azul = 128 + (((int)imagen->pixeles[fila][columna].azul - 128) * (100 - porcentaje)) / 100;

            imagen->pixeles[fila][columna].rojo = (unsigned char)rojo;
            imagen->pixeles[fila][columna].verde = (unsigned char)verde;
            imagen->pixeles[fila][columna].azul = (unsigned char)azul;
        }
    }
}

bool ejecutarEspejarHorizontal(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;
    (void)porcentaje;

    copia = copiarImagen(imagen);
    if (copia == NULL) {
        fprintf(stderr, "No hay memoria suficiente para aplicar %s.\n", nombrefiltro);
        return false;
    }

    espejar_horizontal(copia);

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    if (!generarBMP(destino, copia)) {
        fprintf(stderr, "No se pudo crear el archivo %s.\n", destino);
        liberarImagen(copia);
        return false;
    }

    liberarImagen(copia);
    return true;
}

void espejar_horizontal(Imagen *imagen)
{
    int fila, columna;
    int alto = imagen->cabeceraImagen.alto;
    int ancho = imagen->cabeceraImagen.ancho;
    int altoAbs = (alto < 0) ? -alto : alto;
    Pixel aux;

    for (fila = 0; fila < altoAbs; fila++) {
        for (columna = 0; columna < ancho / 2; columna++) {
            aux = imagen->pixeles[fila][columna];
            imagen->pixeles[fila][columna] = imagen->pixeles[fila][ancho - 1 - columna];
            imagen->pixeles[fila][ancho - 1 - columna] = aux;
        }
    }
}

bool ejecutarEspejarVertical(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch)
{
    char destino[TAM_MAX];
    Imagen *copia;
    (void)porcentaje;

    copia = copiarImagen(imagen);
    if (copia == NULL) {
        fprintf(stderr, "No hay memoria suficiente para aplicar %s.\n", nombrefiltro);
        return false;
    }

    espejar_vertical(copia);

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombrefiltro, nomarch);
    if (!generarBMP(destino, copia)) {
        fprintf(stderr, "No se pudo crear el archivo %s.\n", destino);
        liberarImagen(copia);
        return false;
    }

    liberarImagen(copia);
    return true;
}

void espejar_vertical(Imagen *imagen)
{
    int fila;
    int alto = imagen->cabeceraImagen.alto;
    int altoAbs = (alto < 0) ? -alto : alto;
    Pixel *aux;

    for (fila = 0; fila < altoAbs / 2; fila++) {
        aux = imagen->pixeles[fila];
        imagen->pixeles[fila] = imagen->pixeles[altoAbs - 1 - fila];
        imagen->pixeles[altoAbs - 1 - fila] = aux;
    }
}
