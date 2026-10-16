#include "funciones_castillo.h"

// [APRENDER] ===================================================================
// [APRENDER] NEGATIVO  (TU PRIMERA TAREA)
// [APRENDER] ===================================================================
// [APRENDER] Idea: invertir cada color. Lo que era 0 pasa a 255, 255 pasa a 0,
// [APRENDER] 100 pasa a 155. Formula: nuevo = 255 - viejo, para b, g y r.
// [APRENDER]
// [APRENDER] Esta funcion NO modifica la original (es const): trabaja sobre una copia
// [APRENDER] y devuelve esa copia. Quien la llame es responsable de liberarla con
// [APRENDER] LiberarImagen.
sImagen* Negativo(const sImagen *original)
{
    // [APRENDER] Este filtro no usa porcentaje. (void)parametro le dice al
    // [APRENDER] compilador "se que no lo uso" para que no avise (warning).

    // [APRENDER] Empiezo con una copia exacta de la imagen. Si falla devuelve NULL.
    sImagen *resultado = CopiarImagen(original);
    if(resultado == NULL)
        return NULL;

    // [APRENDER] Doble for = recorrer toda la matriz: i = fila, j = columna.
    for(int i = 0; i < resultado->alto; i++)
    {
        for(int j = 0; j < resultado->ancho; j++)
        {
            // TODO (Castillo): invertir b, g y r de resultado->pixeles[i][j]
            // [APRENDER] Pista: son 3 lineas, una por color. Por ejemplo para el rojo:
            // [APRENDER]   resultado->pixeles[i][j].r = 255 - resultado->pixeles[i][j].r;
            // [APRENDER] (a proposito NO lo dejo escrito: hacelo vos para el b, g y r).

            resultado->pixeles[i][j].r=255-resultado->pixeles[i][j].r;
            resultado->pixeles[i][j].g=255-resultado->pixeles[i][j].g;
            resultado->pixeles[i][j].b=255-resultado->pixeles[i][j].b;
        }
    }

    return resultado;
}

sImagen* EscalaDeGrises(const sImagen *original)
{

    // [APRENDER] Empiezo con una copia exacta de la imagen. Si falla devuelve NULL.
    sImagen *resultado = CopiarImagen(original);
    if(resultado == NULL)
        return NULL;

    // [APRENDER] Doble for = recorrer toda la matriz: i = fila, j = columna.
    for(int i = 0; i < resultado->alto; i++)
    {
        for(int j = 0; j < resultado->ancho; j++)
        {
            int promedio=(resultado->pixeles[i][j].r+resultado->pixeles[i][j].b+resultado->pixeles[i][j].g)/3;
            resultado->pixeles[i][j].r=promedio;
            resultado->pixeles[i][j].g=promedio;
            resultado->pixeles[i][j].b=promedio;

        }
    }

    return resultado;
}


sImagen* TonalidadAzul(const sImagen *original, int parametro)
{
    // [APRENDER] Este filtro no usa porcentaje. (void)parametro le dice al
    // [APRENDER] compilador "se que no lo uso" para que no avise (warning).


    // [APRENDER] Empiezo con una copia exacta de la imagen. Si falla devuelve NULL.
    sImagen *resultado = CopiarImagen(original);
    if(resultado == NULL)
        return NULL;



    // [APRENDER] Doble for = recorrer toda la matriz: i = fila, j = columna.
    for(int i = 0; i < resultado->alto; i++)
    {
        for(int j = 0; j < resultado->ancho; j++)
        {
            int nuevo=resultado->pixeles[i][j].b+resultado->pixeles[i][j].b*parametro/100;
            if(nuevo>255)
                nuevo=255;
            resultado->pixeles[i][j].b=nuevo;
        }
    }

    return resultado;
}
sImagen* TonalidadRojo(const sImagen *original, int parametro)
{
    // [APRENDER] Este filtro no usa porcentaje. (void)parametro le dice al
    // [APRENDER] compilador "se que no lo uso" para que no avise (warning).


    // [APRENDER] Empiezo con una copia exacta de la imagen. Si falla devuelve NULL.
    sImagen *resultado = CopiarImagen(original);
    if(resultado == NULL)
        return NULL;



    // [APRENDER] Doble for = recorrer toda la matriz: i = fila, j = columna.
    for(int i = 0; i < resultado->alto; i++)
    {
        for(int j = 0; j < resultado->ancho; j++)
        {
            int nuevo=resultado->pixeles[i][j].r+resultado->pixeles[i][j].r*parametro/100;
            if(nuevo>255)
                nuevo=255;
            resultado->pixeles[i][j].r=nuevo;
        }
    }

    return resultado;
}
sImagen* TonalidadVerde(const sImagen *original, int parametro)
{
    // [APRENDER] Este filtro no usa porcentaje. (void)parametro le dice al
    // [APRENDER] compilador "se que no lo uso" para que no avise (warning).


    // [APRENDER] Empiezo con una copia exacta de la imagen. Si falla devuelve NULL.
    sImagen *resultado = CopiarImagen(original);
    if(resultado == NULL)
        return NULL;



    // [APRENDER] Doble for = recorrer toda la matriz: i = fila, j = columna.
    for(int i = 0; i < resultado->alto; i++)
    {
        for(int j = 0; j < resultado->ancho; j++)
        {
            int nuevo=resultado->pixeles[i][j].g+resultado->pixeles[i][j].g*parametro/100;
            if(nuevo>255)
                nuevo=255;
            resultado->pixeles[i][j].g=nuevo;
        }
    }

    return resultado;
}

