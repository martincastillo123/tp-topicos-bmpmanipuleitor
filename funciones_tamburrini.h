#ifndef FUNCIONES_TAMBURRINI_H_INCLUDED
#define FUNCIONES_TAMBURRINI_H_INCLUDED
#include "compartido.h"

bool ejecutarNegativo(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarAumentarContraste(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarAchicar(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarRotarDerecha(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarRotarIzquierda(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarConcatenar(Imagen *imagen1, int argc, char *argv[], const char *nombrefiltro, char *nomarch);

void negativo(Imagen *imagen);
void aumentar_contraste(Imagen *imagen, int porcentaje);
bool rotarMatrizDerecha(Imagen *imagen);
bool rotarMatrizIzquierda(Imagen *imagen);
Imagen *concatenarMatrices(const Imagen *img1, const Imagen *img2, bool esHorizontal);
bool achicarMatriz(Imagen *imagen, int porcentaje);
#endif // FUNCIONES_TAMBURRINI_H_INCLUDED
