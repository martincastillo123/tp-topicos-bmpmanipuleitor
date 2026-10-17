/*Tamburrini, Nicolas
46573624
entrega:Si
Aguila, Mateo
46563915
entrega:Si
Castillo, Martin
46196957
entrega:Si
Saquilan, Tomas
42283011
entrega:Si
*/
#include "funciones_grupo.h"
int main(int argc, char *argv[])
{
    char *nombreArchivo;
    Imagen *imagen;

    if (argc < 3) {
        fprintf(stderr, "Faltan argumentos.\n");
        return 1;
    }

    nombreArchivo = buscarArchivoBMP(argc, argv);

    if (nombreArchivo == NULL) {
        fprintf(stderr, "No se encontro un archivo BMP.\n");
        return 1;
    }

    imagen = cargarBMP(nombreArchivo);

    if (imagen == NULL) {
        fprintf(stderr, "No se pudo cargar la imagen.\n");
        return 1;
    }

    procesarArgumentos(argc, argv, imagen, nombreArchivo);

    liberarImagen(imagen);

    printf("\n SI DIOS QUIERE....");

    return 0;
}
