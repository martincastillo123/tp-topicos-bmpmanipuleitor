// [APRENDER] ===== QUE ES UN .h =====
// [APRENDER] Un .h (header) es el 'indice' o 'menu' de un .c: dice QUE funciones y tipos
// [APRENDER] existen, pero no COMO funcionan. El como esta en el .c con el mismo nombre.
// [APRENDER] Quien quiera usar estas funciones escribe #include "compartido.h" y el
// [APRENDER] compilador pega este texto ahi.
// [APRENDER]
// [APRENDER] #ifndef / #define / #endif = 'guarda de inclusion'. Evita que el mismo .h se
// [APRENDER] pegue dos veces en un mismo .c (daria error de tipos repetidos).
#ifndef COMPARTIDO_H_INCLUDED
#define COMPARTIDO_H_INCLUDED

// [APRENDER] #include <...> = librerias del sistema. #include "..." = archivos del proyecto.
// [APRENDER] stdio.h: FILE, fopen, fread, fwrite, printf.  stdlib.h: malloc, free.
// [APRENDER] string.h: memset.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////// ESTRUCTURA DE DATOS ////////

// [APRENDER] typedef struct {...} sPixel;  = 'creo un tipo nuevo llamado sPixel' formado por
// [APRENDER] 3 campos. unsigned char = numero de 0 a 255 (ocupa 1 byte): por eso cada color
// [APRENDER] va de 0 a 255. El orden es b,g,r (no r,g,b) porque asi lo guarda el BMP.
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
    // [APRENDER] sPixel ** = 'puntero a puntero a sPixel' = LA MATRIZ. Es un arreglo de punteros
    // [APRENDER] y cada puntero apunta a una fila de pixeles. Se usa: pixeles[fila][columna].
    // [APRENDER] La memoria se reserva con malloc en CrearImagen y se libera en LiberarImagen.
    sPixel **pixeles;
}sImagen;

// [APRENDER] Reflejo del encabezado del archivo .bmp: datos SOBRE la imagen que van antes de
// [APRENDER] los pixeles. unsigned short = 2 bytes, unsigned int = 4 bytes (deben coincidir
// [APRENDER] con los tamanios del PDF). 'inicioDatos' dice en que byte empiezan los pixeles:
// [APRENDER] en tus imagenes de prueba es 138, no 54.
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



// [APRENDER] Estos son PROTOTIPOS: solo la 'firma' de cada funcion (que devuelve, nombre,
// [APRENDER] parametros) terminada en ;. Con eso otros .c pueden LLAMARLAS sin ver su codigo.
// [APRENDER] El codigo real esta en compartido.c. Si una funcion se declara aca pero nadie
// [APRENDER] la define, el linker da el error 'undefined reference'.
// [APRENDER] const = promete no modificar lo que apunta el puntero.
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

// [APRENDER] Cierra el #ifndef del principio.
#endif // COMPARTIDO_H_INCLUDED
