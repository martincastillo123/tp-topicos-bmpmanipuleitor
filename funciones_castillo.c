#include "funciones_castillo.h"

bool Negativo(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo)
{
    char destino[TAM_MAX];
    Imagen *resultado = copiarImagen(original);
    if(resultado == NULL)
        return false;

    int alto = resultado->cabeceraImagen.alto;
    if(alto < 0)
        alto = -alto;

    for(int i = 0; i < alto; i++)
    {
        for(int j = 0; j < resultado->cabeceraImagen.ancho; j++)
        {
            resultado->pixeles[i][j].rojo=255-resultado->pixeles[i][j].rojo;
            resultado->pixeles[i][j].verde=255-resultado->pixeles[i][j].verde;
            resultado->pixeles[i][j].azul=255-resultado->pixeles[i][j].azul;
        }
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombreFiltro, nombreArchivo);
    generarBMP(destino, resultado);
    liberarImagen(resultado);
    return true;
}

bool EscalaDeGrises(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo)
{
    char destino[TAM_MAX];
    Imagen *resultado = copiarImagen(original);
    if(resultado == NULL)
        return false;

    int alto = resultado->cabeceraImagen.alto;
    if(alto < 0)
        alto = -alto;

    for(int i = 0; i < alto; i++)
    {
        for(int j = 0; j < resultado->cabeceraImagen.ancho; j++)
        {
            int promedio=(resultado->pixeles[i][j].azul+resultado->pixeles[i][j].verde+resultado->pixeles[i][j].rojo)/3;

            resultado->pixeles[i][j].azul=promedio;
            resultado->pixeles[i][j].verde=promedio;
            resultado->pixeles[i][j].rojo=promedio;
        }
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombreFiltro, nombreArchivo);
    generarBMP(destino, resultado);
    liberarImagen(resultado);
    return true;
}

bool ReducirContraste(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo)
{
    char destino[TAM_MAX];
    Imagen *resultado = copiarImagen(original);
    if(resultado == NULL)
        return false;

    int alto = resultado->cabeceraImagen.alto;
    if(alto < 0)
        alto = -alto;

    for(int i = 0; i < alto; i++)
    {
        for(int j = 0; j < resultado->cabeceraImagen.ancho; j++)
        {
            resultado->pixeles[i][j].rojo=128+(resultado->pixeles[i][j].rojo-128)*(100-parametro)/100;
            resultado->pixeles[i][j].verde=128+(resultado->pixeles[i][j].verde-128)*(100-parametro)/100;
            resultado->pixeles[i][j].azul=128+(resultado->pixeles[i][j].azul-128)*(100-parametro)/100;
        }
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombreFiltro, nombreArchivo);
    generarBMP(destino, resultado);
    liberarImagen(resultado);
    return true;
}

bool Recortar(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo)
{
    char destino[TAM_MAX];

    int nuevoAlto = original->cabeceraImagen.alto;
    if(nuevoAlto < 0)
        nuevoAlto = -nuevoAlto;
    nuevoAlto=nuevoAlto*parametro/100;
    int nuevoAncho=original->cabeceraImagen.ancho*parametro/100;

    if(nuevoAlto < 1)
        nuevoAlto = 1;
    if(nuevoAncho < 1)
        nuevoAncho = 1;

    Imagen *resultado = malloc(sizeof(Imagen));
    if(resultado == NULL)
        return false;

    resultado->cabeceraArchivo = original->cabeceraArchivo;
    resultado->cabeceraImagen = original->cabeceraImagen;
    resultado->cabeceraImagen.alto = nuevoAlto;
    resultado->cabeceraImagen.ancho = nuevoAncho;

    if(!reservarMatriz(resultado))
    {
        free(resultado);
        return false;
    }

    for(int i = 0; i < nuevoAlto; i++)
    {
        for(int j = 0; j < nuevoAncho; j++)
            resultado->pixeles[i][j]=original->pixeles[i][j];
    }

    generarNombre(destino, TAM_MAX, NOMBRE_GRUPO, nombreFiltro, nombreArchivo);
    generarBMP(destino, resultado);
    liberarImagen(resultado);
    return true;
}
