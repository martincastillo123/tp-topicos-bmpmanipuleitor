#ifndef ESTRUCTURAS_H_INCLUDED
#define ESTRUCTURAS_H_INCLUDED
// Estructura sugerida, su uso es opcional.
typedef struct
{
    unsigned char pixel[3];
}t_pixel;

typedef struct
{
    unsigned int tamArchivo;
    unsigned int tamEncabezado;
    unsigned int ancho;
    unsigned int alto;
    unsigned short profundidad;
}t_metadata;
#endif // ESTRUCTURAS_H_INCLUDED
