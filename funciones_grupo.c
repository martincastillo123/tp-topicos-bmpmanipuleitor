#include "funciones_grupo.h"

// Tabla global de filtros simples y con porcentaje asignando las funciones de cada integrante
static Filtro filtros[] = {

    {"--negativo", ejecutarNegativo, false},
    {"--aumentar-contraste", ejecutarAumentarContraste, false},
    {"--rotar-derecha", ejecutarRotarDerecha, false},
    {"--rotar-izquierda", ejecutarRotarIzquierda, false},
    {"--achicar", ejecutarAchicar, false},

};
static int totalFiltros = sizeof(filtros) / sizeof(filtros[0]);

Filtro *buscarFiltro(const char *nombre) {
    for (int i = 0; i < totalFiltros; i++) {
        if (strcmp(filtros[i].nombre, nombre) == 0) {
            return &filtros[i];
        }
    }
    return NULL;
}

void procesarArgumentos(int argc, char *argv[], Imagen *imagen, char *nomarch) {
    int porcentaje;
    char nombre[50];
    Filtro *filtro;

    for (int i = 1; i < argc; i++) {
        if (esArchivoBMP(argv[i])) continue;

        if (strcmp(argv[i], "--concatenar-horizontal") == 0 || strcmp(argv[i], "--concatenar-vertical") == 0) {
            ejecutarConcatenar(imagen, argc, argv, argv[i], nomarch);
            continue;
        }

        if (obtenerFiltroConPorcentaje(argv[i], nombre, &porcentaje)) {
            filtro = buscarFiltro(nombre);
            if (!filtro) { fprintf(stderr, "Filtro invalido: %s\n", nombre); continue; }
            if (porcentaje < 0 || porcentaje > 100) { fprintf(stderr, "Porcentaje invalido.\n"); continue; }
            if (filtro->ejecutado) continue;

            if (filtro->funcion(imagen, porcentaje, nombre, nomarch)) {
                filtro->ejecutado = true;
            }
        } else {
            filtro = buscarFiltro(argv[i]);
            if (!filtro) { fprintf(stderr, "Argumento invalido: %s\n", argv[i]); continue; }
            if (filtro->ejecutado) continue;

            const char *nombreLimpio = argv[i];
            if (strncmp(nombreLimpio, "--", 2) == 0) nombreLimpio += 2;

            if (filtro->funcion(imagen, 0, (char*)nombreLimpio, nomarch)) {
                filtro->ejecutado = true;
            }
        }
    }
}
