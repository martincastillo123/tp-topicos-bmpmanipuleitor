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

#include <stdio.h>
#include "funciones_grupo.h"
#include "compartido.h"

int solucion(int argc, char* argv[])
{
    /*
        PROVISORIO: recorrido de prueba del nucleo BMP (cargar, copiar, escribir).
        Reemplazar por el parser de argumentos y la ejecucion de cada filtro.
    */
    if(argc < 2)
    {
        printf("\nUso: %s [--filtros...] archivo.bmp", argv[0]);
        return 1;
    }

    printf("\nCargando: %s...", argv[1]);
    sImagen *imagen = CargarImagen(argv[1]);
    if(imagen == NULL)
    {
        printf("\nError: no se pudo cargar la imagen.\n");
        return 1;
    }

    ImprimirInfoImagen(imagen);
    printf("\nListo, imagen cargada correctamente.");

    sImagen *copia = CopiarImagen(imagen);
    if(copia == NULL)
    {
        printf("\nError: no se pudo copiar la imagen.");
        LiberarImagen(imagen);
        return 1;
    }

    if(!EscribirBMP("copia.bmp", copia))
    {
        LiberarImagen(copia);
        LiberarImagen(imagen);
        return 1;
    }

    LiberarImagen(copia);
    LiberarImagen(imagen);

    return 0;
}
