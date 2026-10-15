#include "compartido.h"

//////// LECTURA DE BMP ////////

int LeerBMP(const char *nombreArchivo, sEncabezadoBMP *enc)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if(archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo.");
        return 0;
    }

    // Leer el encabezado
    LeerEncabezado(archivo, enc);

    // Verificar que sea un BMP
    if(!VerificarBMP(enc))
    {
        fclose(archivo);
        return 0;
    }

    fclose(archivo);
    return 1;
}


void LeerEncabezado(FILE *archivo, sEncabezadoBMP *enc)
{
    fread(enc->tipo, 1, 2, archivo);
    fread(&enc->tamanioArchivo, 4, 1, archivo);
    fread(&enc->reservado1, 2, 1, archivo);
    fread(&enc->reservado2, 2, 1, archivo);
    fread(&enc->inicioDatos, 4, 1, archivo);
    fread(&enc->tamanioCabecera, 4, 1, archivo);
    fread(&enc->ancho, 4, 1, archivo);
    fread(&enc->alto, 4, 1, archivo);
    fread(&enc->planos, 2, 1, archivo);
    fread(&enc->bitsPorPixel, 2, 1, archivo);
    fread(&enc->compresion, 4, 1, archivo);
    fread(&enc->tamanioImagen, 4, 1, archivo);
    fread(&enc->resolucionH, 4, 1, archivo);
    fread(&enc->resolucionV, 4, 1, archivo);
    fread(&enc->coloresUsados, 4, 1, archivo);
    fread(&enc->coloresImportantes, 4, 1, archivo);
}

int VerificarBMP(const sEncabezadoBMP *enc)
{
    if(enc == NULL)
    {
        printf("\nError: encabezado NULL.");
        return 0;
    }

    if(enc->tipo[0] != 'B' || enc->tipo[1] != 'M')
    {
        printf("\nError: '%c%c' no es un BMP valido (se esperaba 'BM').", enc->tipo[0], enc->tipo[1]);
        return 0;
    }

    if(enc->bitsPorPixel != 24)
    {
        printf("\nError: solo se soportan BMP de 24 bits.");
        return 0;
    }

    if(enc->compresion != 0)
    {
        printf("\nError: solo se soportan BMP sin compresion.");
        return 0;
    }

    if(enc->ancho <= 0 || enc->alto <= 0)
    {
        printf("\nError: dimensiones invalidas.");
        return 0;
    }

    if(enc->planos != 1)
    {
        printf("\nError: numero de planos invalido.");
        return 0;
    }

    return 1;
}


sImagen* CrearImagen(int ancho, int alto)
{
    sImagen *imagen = (sImagen*)malloc(sizeof(sImagen));
    if(imagen == NULL)
    {
        printf("\nError: no se pudo reservar memoria para la imagen.");
        return NULL;
    }

    imagen->ancho = ancho;
    imagen->alto = alto;

    imagen->pixeles = (sPixel**)malloc(alto * sizeof(sPixel*));
    if(imagen->pixeles == NULL)
    {
        printf("\nError: no se pudo reservar memoria para las filas.");
        free(imagen);
        return NULL;
    }

    for(int i=0; i<alto; i++)
    {
        imagen->pixeles[i] = (sPixel*)malloc(ancho * sizeof(sPixel));
        if(imagen->pixeles[i] == NULL)
        {
            printf("\nError: no se pudo reservar memoria para las columnas.");
            for(int j=0; j<i; j++)
            {
                free(imagen->pixeles[j]);
            }
            free(imagen->pixeles);
            free(imagen);
            return NULL;
        }
    }

    return imagen;
}

int CalcularPadding(int ancho)
{
    return (4 - (ancho * 3) % 4) % 4;
}

int CalcularBytesPorFila(int ancho)
{
    return ancho * 3 + CalcularPadding(ancho);
}

int PosicionarEnDatos(FILE *archivo, unsigned int inicioDatos)
{
    if(fseek(archivo, inicioDatos, SEEK_SET) != 0)
    {
        printf("\nError: no se pudo posicionar en el inicio de los datos.");
        return 0;
    }

    return 1;
}

int LeerPixeles(const char *nombreArchivo, sImagen *imagen, const sEncabezadoBMP *enc)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo.");
        return 0;
    }

    if (!PosicionarEnDatos(archivo, enc->inicioDatos))
    {
        fclose(archivo);
        return 0;
    }

    int bytesPorFila = CalcularBytesPorFila(enc->ancho);

    unsigned char *filaBuffer = (unsigned char*)malloc(bytesPorFila);
    if (filaBuffer == NULL)
    {
        printf("\nError: no se pudo reservar memoria para el buffer.");
        fclose(archivo);
        return 0;
    }

    for(int i=0; i<enc->alto; i++)
    {
        size_t leidos = fread(filaBuffer, 1, bytesPorFila, archivo);
        if(leidos != (size_t)bytesPorFila)
        {
            printf("\nError: no se pudieron leer todos los bytes de la fila %d.", i);
            free(filaBuffer);
            fclose(archivo);
            return 0;
        }

        int filaDestino = enc->alto-1-i;
        for(int j=0; j<enc->ancho; j++)
        {
            imagen->pixeles[filaDestino][j].b = filaBuffer[j*3+0];
            imagen->pixeles[filaDestino][j].g = filaBuffer[j*3+1];
            imagen->pixeles[filaDestino][j].r = filaBuffer[j*3+2];
        }
    }

    free(filaBuffer);
    fclose(archivo);
    return 1;
}

void LiberarImagen(sImagen *imagen)
{
    if(imagen == NULL)
        return;

    if(imagen->pixeles != NULL)
    {
        for(int i=0; i<imagen->alto; i++)
        {
            free(imagen->pixeles[i]);
        }

        free(imagen->pixeles);
    }

    free(imagen);
}

sImagen* CargarImagen(const char *nombreArchivo)
{
    sEncabezadoBMP enc;
    if(!LeerBMP(nombreArchivo, &enc))
    {
        printf("\nError: no se pudo leer el encabezado de '%s'.", nombreArchivo);
        return NULL;
    }

    sImagen *imagen = CrearImagen(enc.ancho, enc.alto);
    if(imagen == NULL)
    {
        printf("\nError: no se pudo crear la imagen en memoria.");
        return NULL;
    }

    if(!LeerPixeles(nombreArchivo, imagen, &enc))
    {
        printf("\nError: no se pudieron leer los pixeles de '%s'.", nombreArchivo);
        LiberarImagen(imagen);
        return NULL;
    }

    return imagen;
}

void ImprimirInfoImagen(const sImagen *imagen)
{
    if(imagen == NULL)
    {
        printf("\nImagen: NULL.");
        return;
    }

    printf("\nImagen: %dx%d (%d pixeles)", imagen->ancho, imagen->alto, imagen->ancho * imagen->alto);
}

sImagen* CopiarImagen(const sImagen *original)
{
    if(original == NULL)
    {
        printf("\nError: imagen original NULL.");
        return NULL;
    }

    sImagen *copia = CrearImagen(original->ancho, original->alto);
    if(copia == NULL)
    {
        printf("\nError: no se pudo crear la copia.");
        return NULL;
    }

    for(int i=0; i<original->alto; i++)
    {
        for(int j=0; j<original->ancho; j++)
        {
            copia->pixeles[i][j] = original->pixeles[i][j];
        }
    }

    return copia;
}

void ArmarEncabezado(const sImagen *imagen, sEncabezadoBMP *enc)
{
    if(imagen == NULL || enc == NULL)
        return;

    memset(enc, 0, sizeof(sEncabezadoBMP));

    int bytesPorFila = CalcularBytesPorFila(imagen->ancho);
    unsigned int tamanioImagen = bytesPorFila * imagen->alto;
    unsigned int tamanioArchivo = 54 + tamanioImagen;

    enc->tipo[0] = 'B';
    enc->tipo[1] = 'M';
    enc->tamanioArchivo = tamanioArchivo;
    enc->reservado1 = 0;
    enc->reservado2 = 0;
    enc->inicioDatos = 54;
    enc->tamanioCabecera = 40;
    enc->ancho = imagen->ancho;
    enc->alto = imagen->alto;
    enc->planos = 1;
    enc->bitsPorPixel = 24;
    enc->compresion = 0;
    enc->tamanioImagen = tamanioImagen;
    enc->resolucionH = 2835;
    enc->resolucionV = 2835;
    enc->coloresUsados = 0;
    enc->coloresImportantes = 0;
}

void EscribirEncabezado(FILE *archivo, const sEncabezadoBMP *enc)
{
    if(archivo == NULL || enc == NULL)
        return;

    fwrite(enc->tipo, 1, 2, archivo);
    fwrite(&enc->tamanioArchivo, 4, 1, archivo);
    fwrite(&enc->reservado1, 2, 1, archivo);
    fwrite(&enc->reservado2, 2, 1, archivo);
    fwrite(&enc->inicioDatos, 4, 1, archivo);
    fwrite(&enc->tamanioCabecera, 4, 1, archivo);
    fwrite(&enc->ancho, 4, 1, archivo);
    fwrite(&enc->alto, 4, 1, archivo);
    fwrite(&enc->planos, 2, 1, archivo);
    fwrite(&enc->bitsPorPixel, 2, 1, archivo);
    fwrite(&enc->compresion, 4, 1, archivo);
    fwrite(&enc->tamanioImagen, 4, 1, archivo);
    fwrite(&enc->resolucionH, 4, 1, archivo);
    fwrite(&enc->resolucionV, 4, 1, archivo);
    fwrite(&enc->coloresUsados, 4, 1, archivo);
    fwrite(&enc->coloresImportantes, 4, 1, archivo);
}

int EscribirPixeles(FILE *archivo, const sImagen *imagen)
{
    if(archivo == NULL || imagen == NULL)
        return 0;

    int bytesPorFila = CalcularBytesPorFila(imagen->ancho);
    int padding = CalcularPadding(imagen->ancho);

    unsigned char *filaBuffer = (unsigned char*)malloc(bytesPorFila);
    if(filaBuffer == NULL)
    {
        printf("\nError: no se pudo reservar memoria para el buffer");
        return 0;
    }

    for(int i=0; i<imagen->alto; i++)
    {
        int filaOrigen = imagen->alto-1-i;
        for(int j=0; j<imagen->ancho; j++)
        {
            filaBuffer[j*3+0] = imagen->pixeles[filaOrigen][j].b;
            filaBuffer[j*3+1] = imagen->pixeles[filaOrigen][j].g;
            filaBuffer[j*3+2] = imagen->pixeles[filaOrigen][j].r;
        }

        for(int p=0; p<padding; p++)
        {
            filaBuffer[imagen->ancho*3+p] = 0;
        }

        size_t escritos = fwrite(filaBuffer, 1, bytesPorFila, archivo);
        if(escritos != (size_t)bytesPorFila)
        {
            printf("\nError: no se pudo escribir la fila %d.", i);
            free(filaBuffer);
            return 0;
        }
    }

    free(filaBuffer);
    return 1;
}

int EscribirBMP(const char *nombreArchivo, const sImagen *imagen)
{
    if(imagen == NULL)
    {
        printf("\nError: imagen NULL");
        return 0;
    }

    FILE *archivo = fopen(nombreArchivo, "wb");
    if(archivo == NULL)
    {
        printf("\nError: no se pudo crear '%s'.", nombreArchivo);
        return 0;
    }

    sEncabezadoBMP enc;
    ArmarEncabezado(imagen, &enc);
    EscribirEncabezado(archivo, &enc);

    if(!EscribirPixeles(archivo, imagen))
    {
        fclose(archivo);
        return 0;
    }

    fclose(archivo);

    printf("\nImagen '%s' escrita correctamente (%dx%d).", nombreArchivo, imagen->ancho, imagen->alto);
    return 1;
}

