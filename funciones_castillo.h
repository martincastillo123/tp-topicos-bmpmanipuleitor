#ifndef FUNCIONES_CASTILLO_H_INCLUDED
#define FUNCIONES_CASTILLO_H_INCLUDED

// [APRENDER] Este .h es el "menu" de los filtros de Castillo: avisa que existen.
// [APRENDER] Incluye compartido.h porque los prototipos usan el tipo sImagen.
// [APRENDER] funciones_grupo.h incluye este archivo, y asi funciones_grupo.c puede
// [APRENDER] poner "Negativo" en la tabla sin ver su codigo.
#include "compartido.h"

// [APRENDER] Prototipo: solo la firma. Coincide con tFuncionFiltro de funciones_grupo.c
// [APRENDER] (recibe imagen + int, devuelve imagen). El codigo esta en funciones_castillo.c.
sImagen* Negativo(const sImagen *original);
sImagen* EscalaDeGrises(const sImagen *original);
sImagen* TonalidadAzul(const sImagen *original, int parametro);
sImagen* TonalidadVerde(const sImagen *original, int parametro);
sImagen* TonalidadRojo(const sImagen *original, int parametro);


#endif // FUNCIONES_CASTILLO_H_INCLUDED
