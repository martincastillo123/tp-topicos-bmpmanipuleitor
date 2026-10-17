#ifndef COMPARTIDO_H_INCLUDED
#define COMPARTIDO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define TAM_MAX 256
#define NOMBRE_GRUPO "EDITOR"

typedef struct {
    unsigned char azul;
    unsigned char verde;
    unsigned char rojo;
} Pixel;

typedef struct {
    unsigned short tipo;
    unsigned int tamanoArchivo;
    unsigned short reservado1;
    unsigned short reservado2;
    unsigned int inicioDatos;
} CabeceraArchivo;

typedef struct {
    unsigned int tamanoCabecera;
    int ancho;
    int alto;
    unsigned short planos;
    unsigned short bitsPorPixel;
    unsigned int compresion;
    unsigned int tamanoImagen;
    int resolucionHorizontal;
    int resolucionVertical;
    unsigned int coloresTabla;
    unsigned int coloresImportantes;
} CabeceraImagen;

typedef struct {
    CabeceraArchivo cabeceraArchivo;
    CabeceraImagen cabeceraImagen;
    Pixel **pixeles;
} Imagen;

// Declaraciones de funciones compartidas
Imagen *cargarBMP(const char *nombreArchivo);
bool generarBMP(const char *nombreArchivoSalida, const Imagen *imagen);
bool reservarMatriz(Imagen *imagen);
void liberarImagen(Imagen *imagen);
Imagen *copiarImagen(const Imagen *original);
bool esArchivoBMP(const char *nombre);
bool obtenerFiltroConPorcentaje(const char *argumento, char *nombre, int *porcentaje);
void generarNombre(char *destino, int tamMax, const char *grupo, const char *filtro, const char *original);
char *buscarArchivoBMP(int argc, char *argv[]);
#endif // COMPARTIDO_H_INCLUDED
