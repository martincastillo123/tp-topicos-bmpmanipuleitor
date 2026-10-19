#ifndef FUNCIONES_GRUPO_H_INCLUDED
#define FUNCIONES_GRUPO_H_INCLUDED

#include "compartido.h"

#include "funciones_tamburrini.h"
#include "funciones_castillo.h"
/*
#include funciones_aguila.h
#include funciones_saquilan.h
*/
typedef bool (*FuncionFiltro)(Imagen *, int, char *, char *);

typedef struct {
    char nombre[50];
    FuncionFiltro funcion;
    bool ejecutado;
} Filtro;

void procesarArgumentos(int argc, char *argv[], Imagen *imagen, char *nomarch);

#endif
