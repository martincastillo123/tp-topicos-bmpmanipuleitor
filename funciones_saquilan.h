#ifndef FUNCIONES_SAQUILAN_H_INCLUDED
#define FUNCIONES_SAQUILAN_H_INCLUDED
#include "compartido.h"

bool ejecutarReducirContraste(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarEspejarHorizontal(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);
bool ejecutarEspejarVertical(Imagen *imagen, int porcentaje, char* nombrefiltro, char* nomarch);

void reducir_contraste(Imagen *imagen, int porcentaje);
void espejar_horizontal(Imagen *imagen);
void espejar_vertical(Imagen *imagen);
#endif // FUNCIONES_SAQUILAN_H_INCLUDED
