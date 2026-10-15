#ifndef COMPARTIDO_H_INCLUDED
#define COMPARTIDO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////// ESTRUCTURA DE DATOS ////////

// Un pixel: 3 bytes (BGR)
typedef struct
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
}sPixel;

// Una imagen: ancho, alto y matriz de pixeles
typedef struct
{
    int alto;
    int ancho;
    sPixel **pixeles;
}sImagen;

//Encabezado BMP (54 bytes)
typedef struct
{
    unsigned char tipo[2]; //BM
    unsigned int tamanioArchivo;
    unsigned short reservado1;
    unsigned short reservado2;
    unsigned int inicioDatos;
    unsigned int tamanioCabecera;
    int ancho;
    int alto;
    unsigned short planos;
    unsigned short bitsPorPixel;
    unsigned int compresion;
    unsigned int tamanioImagen;
    int resolucionH;
    int resolucionV;
    unsigned int coloresUsados;
    unsigned int coloresImportantes;
}sEncabezadoBMP;



//////// FUNCIONES ////////
void LeerEncabezado(FILE *archivo, sEncabezadoBMP *enc);
int VerificarBMP(const sEncabezadoBMP *enc);
int LeerBMP(const char *nombreArchivo, sEncabezadoBMP *enc);
sImagen* CrearImagen(int ancho, int alto);
int CalcularPadding(int ancho);
int CalcularBytesPorFila(int ancho);
int PosicionarEnDatos(FILE *archivo, unsigned int inicioDatos);
int LeerPixeles(const char *nombreArchivo, sImagen *imagen, const sEncabezadoBMP *enc);
void LiberarImagen(sImagen *imagen);
sImagen* CargarImagen(const char *nombreArchivo);
void ImprimirInfoImagen(const sImagen *original);
sImagen* CopiarImagen(const sImagen *original);
void ArmarEncabezado(const sImagen *imagen, sEncabezadoBMP *enc);
void EscribirEncabezado(FILE *archivo, const sEncabezadoBMP *enc);
int EscribirPixeles(FILE *archivo, const sImagen *imagen);
int EscribirBMP(const char *nombreArchivo, const sImagen *imagen);

#endif // COMPARTIDO_H_INCLUDED
