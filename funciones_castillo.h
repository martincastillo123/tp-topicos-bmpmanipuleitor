#ifndef FUNCIONES_CASTILLO_H_INCLUDED
#define FUNCIONES_CASTILLO_H_INCLUDED
#include "compartido.h"

bool Negativo(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo);
bool EscalaDeGrises(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo);
bool ReducirContraste(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo);
bool Recortar(Imagen *original, int parametro, char *nombreFiltro, char *nombreArchivo);

#endif
