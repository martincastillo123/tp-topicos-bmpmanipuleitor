#include "compartido.h"

int leer16(FILE *archivo, unsigned short *valor) {
    unsigned char bytes[2];
    if (fread(bytes, 1, 2, archivo) != 2) return 0;
    *valor = (unsigned short)((unsigned int)bytes[0] | ((unsigned int)bytes[1] << 8));
    return 1;
}

int leer32(FILE *archivo, unsigned int *valor) {
    unsigned char bytes[4];
    if (fread(bytes, 1, 4, archivo) != 4) return 0;
    *valor = (unsigned int)bytes[0] | ((unsigned int)bytes[1] << 8) | ((unsigned int)bytes[2] << 16) | ((unsigned int)bytes[3] << 24);
    return 1;
}

bool leerCabeceraArchivo(FILE* archivo, Imagen* imagen) {
    if (!leer16(archivo, &imagen->cabeceraArchivo.tipo) ||
        !leer32(archivo, &imagen->cabeceraArchivo.tamanoArchivo) ||
        !leer16(archivo, &imagen->cabeceraArchivo.reservado1) ||
        !leer16(archivo, &imagen->cabeceraArchivo.reservado2) ||
        !leer32(archivo, &imagen->cabeceraArchivo.inicioDatos)) {
        return false;
    }
    return true;
}

bool leerCabeceraImagen(FILE *archivo, Imagen *imagen) {
    unsigned int valor32;
    if (!leer32(archivo, &imagen->cabeceraImagen.tamanoCabecera) ||
        !leer32(archivo, &valor32)) return false;
    imagen->cabeceraImagen.ancho = (int)valor32;

    if (!leer32(archivo, &valor32)) return false;
    imagen->cabeceraImagen.alto = (int)valor32;

    if (!leer16(archivo, &imagen->cabeceraImagen.planos) ||
        !leer16(archivo, &imagen->cabeceraImagen.bitsPorPixel) ||
        !leer32(archivo, &imagen->cabeceraImagen.compresion) ||
        !leer32(archivo, &imagen->cabeceraImagen.tamanoImagen) ||
        !leer32(archivo, &valor32)) return false;
    imagen->cabeceraImagen.resolucionHorizontal = (int)valor32;

    if (!leer32(archivo, &valor32)) return false;
    imagen->cabeceraImagen.resolucionVertical = (int)valor32;

    if (!leer32(archivo, &imagen->cabeceraImagen.coloresTabla) ||
        !leer32(archivo, &imagen->cabeceraImagen.coloresImportantes)) return false;

    return true;
}

bool validarFormato(Imagen *imagen) {
    if (imagen->cabeceraArchivo.tipo != 0x4D42) return false;
    if (imagen->cabeceraImagen.tamanoCabecera != 40 && imagen->cabeceraImagen.tamanoCabecera != 124) return false;
    if (imagen->cabeceraImagen.planos != 1 || imagen->cabeceraImagen.bitsPorPixel != 24 || imagen->cabeceraImagen.compresion != 0) return false;
    if (imagen->cabeceraImagen.ancho <= 0 || imagen->cabeceraImagen.alto == 0 || imagen->cabeceraArchivo.inicioDatos < 54) return false;
    return true;
}

Imagen *cargarBMP(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (!archivo) return NULL;

    Imagen *imagen = malloc(sizeof(Imagen));
    if (!imagen) { fclose(archivo); return NULL; }

    if (!leerCabeceraArchivo(archivo, imagen) || !leerCabeceraImagen(archivo, imagen) || !validarFormato(imagen)) {
        free(imagen); fclose(archivo); return NULL;
    }

    long bytesFaltantes = (long)imagen->cabeceraArchivo.inicioDatos - 54;
    while (bytesFaltantes > 0) {
        unsigned char dummy;
        if (fread(&dummy, 1, 1, archivo) != 1) {
            free(imagen); fclose(archivo); return NULL;
        }
        bytesFaltantes--;
    }

    if (imagen->cabeceraImagen.tamanoCabecera > 40) {
        imagen->cabeceraImagen.tamanoCabecera = 40;
        imagen->cabeceraArchivo.inicioDatos = 54;
    }

    if (!reservarMatriz(imagen)) {
        free(imagen); fclose(archivo); return NULL;
    }

    int ancho = imagen->cabeceraImagen.ancho;
    int alto = imagen->cabeceraImagen.alto;
    int altoAbs = (alto < 0) ? -alto : alto;
    int padding = (4 - (ancho * 3) % 4) % 4;

    for (int fila = altoAbs - 1; fila >= 0; fila--) {
        for (int col = 0; col < ancho; col++) {
            if (fread(&imagen->pixeles[fila][col].azul, 1, 1, archivo) != 1 ||
                fread(&imagen->pixeles[fila][col].verde, 1, 1, archivo) != 1 ||
                fread(&imagen->pixeles[fila][col].rojo, 1, 1, archivo) != 1) {
                liberarImagen(imagen); fclose(archivo); return NULL;
            }
        }
        if (padding > 0) {
            unsigned char relleno[3];
            if (fread(relleno, 1, padding, archivo) != (size_t)padding) {
                liberarImagen(imagen); fclose(archivo); return NULL;
            }
        }
    }

    fclose(archivo);
    return imagen;
}

bool generarBMP(const char *nombreArchivoSalida, const Imagen *imagen) {
    FILE *archivo = fopen(nombreArchivoSalida, "wb");
    if (!archivo) return false;

    fwrite(&imagen->cabeceraArchivo.tipo, sizeof(unsigned short), 1, archivo);
    fwrite(&imagen->cabeceraArchivo.tamanoArchivo, sizeof(unsigned int), 1, archivo);
    fwrite(&imagen->cabeceraArchivo.reservado1, sizeof(unsigned short), 1, archivo);
    fwrite(&imagen->cabeceraArchivo.reservado2, sizeof(unsigned short), 1, archivo);
    fwrite(&imagen->cabeceraArchivo.inicioDatos, sizeof(unsigned int), 1, archivo);

    fwrite(&imagen->cabeceraImagen.tamanoCabecera, sizeof(unsigned int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.ancho, sizeof(int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.alto, sizeof(int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.planos, sizeof(unsigned short), 1, archivo);
    fwrite(&imagen->cabeceraImagen.bitsPorPixel, sizeof(unsigned short), 1, archivo);
    fwrite(&imagen->cabeceraImagen.compresion, sizeof(unsigned int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.tamanoImagen, sizeof(unsigned int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.resolucionHorizontal, sizeof(int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.resolucionVertical, sizeof(int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.coloresTabla, sizeof(unsigned int), 1, archivo);
    fwrite(&imagen->cabeceraImagen.coloresImportantes, sizeof(unsigned int), 1, archivo);

    int ancho = imagen->cabeceraImagen.ancho;
    int alto = imagen->cabeceraImagen.alto;
    int altoAbs = (alto < 0) ? -alto : alto;
    int padding = (4 - (ancho * 3) % 4) % 4;
    unsigned char relleno[3] = {0, 0, 0};

    for (int fila = altoAbs - 1; fila >= 0; fila--) {
        for (int col = 0; col < ancho; col++) {
            fwrite(&imagen->pixeles[fila][col].azul, sizeof(unsigned char), 1, archivo);
            fwrite(&imagen->pixeles[fila][col].verde, sizeof(unsigned char), 1, archivo);
            fwrite(&imagen->pixeles[fila][col].rojo, sizeof(unsigned char), 1, archivo);
        }
        if (padding > 0) {
            fwrite(relleno, sizeof(unsigned char), padding, archivo);
        }
    }

    fclose(archivo);
    return true;
}

bool reservarMatriz(Imagen *imagen) {
    int alto = imagen->cabeceraImagen.alto;
    if (alto < 0) alto = -alto;
    int ancho = imagen->cabeceraImagen.ancho;

    imagen->pixeles = malloc(alto * sizeof(Pixel *));
    if (!imagen->pixeles) return false;

    for (int fila = 0; fila < alto; fila++) {
        imagen->pixeles[fila] = malloc(ancho * sizeof(Pixel));
        if (!imagen->pixeles[fila]) {
            while (fila > 0) free(imagen->pixeles[--fila]);
            free(imagen->pixeles);
            imagen->pixeles = NULL;
            return false;
        }
    }
    return true;
}

void liberarImagen(Imagen *imagen) {
    if (!imagen) return;
    if (imagen->pixeles) {
        int alto = imagen->cabeceraImagen.alto;
        if (alto < 0) alto = -alto;
        for (int i = 0; i < alto; i++) {
            free(imagen->pixeles[i]);
        }
        free(imagen->pixeles);
    }
    free(imagen);
}

Imagen *copiarImagen(const Imagen *original) {
    if (!original) return NULL;
    Imagen *copia = malloc(sizeof(Imagen));
    if (!copia) return NULL;

    copia->cabeceraArchivo = original->cabeceraArchivo;
    copia->cabeceraImagen = original->cabeceraImagen;
    copia->pixeles = NULL;

    if (!reservarMatriz(copia)) {
        free(copia);
        return NULL;
    }

    int ancho = original->cabeceraImagen.ancho;
    int alto = original->cabeceraImagen.alto;
    int altoAbs = (alto < 0) ? -alto : alto;

    for (int fila = 0; fila < altoAbs; fila++) {
        for (int col = 0; col < ancho; col++) {
            copia->pixeles[fila][col] = original->pixeles[fila][col];
        }
    }
    return copia;
}

bool esArchivoBMP(const char *nombre) {
    int len = strlen(nombre);
    if (len < 4) return false;
    return (strcasecmp(nombre + len - 4, ".bmp") == 0);
}

bool obtenerFiltroConPorcentaje(const char *argumento, char *nombre, int *porcentaje) {
    char *igual = strchr(argumento, '=');
    if (!igual) return false;

    int longitudNombre = igual - argumento;
    if (longitudNombre >= 50) longitudNombre = 49;
    strncpy(nombre, argumento, longitudNombre);
    nombre[longitudNombre] = '\0';

    *porcentaje = atoi(igual + 1);
    return true;
}

void generarNombre(char *destino, int tamMax, const char *grupo, const char *filtro, const char *original) {
    snprintf(destino, tamMax, "%s_%s_%s", grupo, filtro, original);
}
char *buscarArchivoBMP(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {

        if (esArchivoBMP(argv[i])) {
            return argv[i];
        }
    }

    return NULL;
}
