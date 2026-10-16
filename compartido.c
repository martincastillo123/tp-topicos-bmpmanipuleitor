// [APRENDER] Este .c tiene el CODIGO (las 'definiciones') de las funciones que compartido.h
// [APRENDER] solo declara. Incluye su propio .h para que el compilador verifique que las
// [APRENDER] firmas coinciden.
#include "compartido.h"

//////// LECTURA DE BMP ////////

// [APRENDER] LeerBMP: abre el archivo, lee el encabezado, lo valida y cierra.
// [APRENDER] NO lee pixeles. Devuelve 1 (ok) o 0 (error). 'enc' es parametro de SALIDA.
int LeerBMP(const char *nombreArchivo, sEncabezadoBMP *enc)
{
    // [APRENDER] fopen abre el archivo. 'rb' = read binary (sin la 'b' Windows altera ciertos
    // [APRENDER] bytes y rompe el BMP). Devuelve NULL si no pudo abrirlo (no existe, etc.),
    // [APRENDER] por eso se chequea enseguida.
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


// [APRENDER] LeerEncabezado: lee del archivo los campos del encabezado y los deja en *enc.
// [APRENDER] Recibe el FILE* YA ABIERTO por quien la llama (LeerBMP); no hace fopen/fclose.
// [APRENDER] 'enc' es un puntero: lo que se escribe en enc->campo queda en la variable del
// [APRENDER] que llamo a la funcion (parametro de SALIDA).
void LeerEncabezado(FILE *archivo, sEncabezadoBMP *enc)
{
    // [APRENDER] fread(donde_guardar, bytes_de_cada_elemento, cuantos_elementos, archivo).
    // [APRENDER] Se lee CAMPO POR CAMPO en el orden exacto del formato BMP. No se lee el struct
    // [APRENDER] entero de golpe porque el compilador puede meter bytes de relleno dentro del
    // [APRENDER] struct y el archivo no los tiene. Cada fread avanza la posicion en el archivo.
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

// [APRENDER] VerificarBMP: valida que sea un BMP que sabemos manejar: empieza con 'BM',
// [APRENDER] 24 bits, sin compresion, ancho/alto positivos y 1 plano.
// [APRENDER] Devuelve 1 si esta bien y 0 si no (en C: 0 = falso, distinto de 0 = verdadero).
// [APRENDER] Imprime el motivo del error.
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


// [APRENDER] CrearImagen: reserva con malloc una imagen VACIA de ancho x alto.
// [APRENDER] Tres capas: (1) el struct sImagen, (2) el arreglo de punteros a filas,
// [APRENDER] (3) cada fila de pixeles. Si algo falla libera lo ya reservado y devuelve NULL.
// [APRENDER] Los pixeles quedan con BASURA hasta que alguien los escriba.
// [APRENDER] Quien la llame debe liberarla con LiberarImagen.
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

    // [APRENDER] Paso 2: un arreglo de 'alto' PUNTEROS (sPixel*), uno por fila. Todavia no
    // [APRENDER] hay pixeles, solo los 'casilleros' que van a apuntar a cada fila.
    imagen->pixeles = (sPixel**)malloc(alto * sizeof(sPixel*));
    if(imagen->pixeles == NULL)
    {
        printf("\nError: no se pudo reservar memoria para las filas.");
        free(imagen);
        return NULL;
    }

    // [APRENDER] Paso 3: a cada puntero le asigno una fila real de 'ancho' pixeles. Recien
    // [APRENDER] ahora existe la matriz y se puede usar imagen->pixeles[fila][columna].
    for(int i=0; i<alto; i++)
    {
        imagen->pixeles[i] = (sPixel*)malloc(ancho * sizeof(sPixel));
        if(imagen->pixeles[i] == NULL)
        {
            printf("\nError: no se pudo reservar memoria para las columnas.");
            // [APRENDER] Si falla la fila i hay que liberar las filas 0..i-1 que ya se reservaron;
            // [APRENDER] si no quedarian ocupando memoria para siempre (memory leak).
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

// [APRENDER] CalcularPadding: cada fila del BMP se rellena con bytes sobrantes hasta que
// [APRENDER] mide un multiplo de 4 bytes. Ejemplo: ancho 359 -> 359*3 = 1077 bytes,
// [APRENDER] 1077 % 4 = 1, faltan 3. Ancho 360 -> 1080 -> multiplo de 4 -> padding 0.
// [APRENDER] El '% 4' de afuera hace que si ya es multiplo de 4 de 0 y no de 4.
int CalcularPadding(int ancho)
{
    return (4 - (ancho * 3) % 4) % 4;
}

// [APRENDER] CalcularBytesPorFila: bytes reales que ocupa UNA fila en el archivo:
// [APRENDER] pixeles * 3 bytes + padding.
int CalcularBytesPorFila(int ancho)
{
    return ancho * 3 + CalcularPadding(ancho);
}

// [APRENDER] PosicionarEnDatos: fseek mueve la posicion de lectura al byte 'inicioDatos'
// [APRENDER] (SEEK_SET = contando desde el principio). Hace falta porque entre el
// [APRENDER] encabezado y los pixeles puede haber bytes extra (en tus imagenes: hasta 138).
int PosicionarEnDatos(FILE *archivo, unsigned int inicioDatos)
{
    if(fseek(archivo, inicioDatos, SEEK_SET) != 0)
    {
        printf("\nError: no se pudo posicionar en el inicio de los datos.");
        return 0;
    }

    return 1;
}

// [APRENDER] LeerPixeles: lee los pixeles del archivo y los vuelca en la matriz 'imagen'
// [APRENDER] ya reservada. Lee una fila completa (con padding) en un buffer temporal y de
// [APRENDER] ahi copia solo los pixeles utiles a la matriz.
int LeerPixeles(const char *nombreArchivo, sImagen *imagen, const sEncabezadoBMP *enc)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo.");
        return 0;
    }

    // [APRENDER] Salta al byte donde empiezan los pixeles (se saltea el encabezado).
    if (!PosicionarEnDatos(archivo, enc->inicioDatos))
    {
        fclose(archivo);
        return 0;
    }

    // [APRENDER] Cuantos bytes ocupa cada fila en el archivo (pixeles + padding).
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

        // [APRENDER] EL BMP GUARDA LA IMAGEN DE ABAJO HACIA ARRIBA: la 1ra fila del archivo es la
        // [APRENDER] de mas abajo de la foto. Para que pixeles[0] sea la fila de ARRIBA (lo
        // [APRENDER] natural), la fila i del archivo va a la posicion alto-1-i de la matriz.
        int filaDestino = enc->alto-1-i;
        for(int j=0; j<enc->ancho; j++)
        {
            // [APRENDER] Cada pixel son 3 bytes seguidos en el buffer: posicion j*3 = azul, +1 = verde,
            // [APRENDER] +2 = rojo. El padding (sobrante al final de la fila) simplemente no se copia.
            imagen->pixeles[filaDestino][j].b = filaBuffer[j*3+0];
            imagen->pixeles[filaDestino][j].g = filaBuffer[j*3+1];
            imagen->pixeles[filaDestino][j].r = filaBuffer[j*3+2];
        }
    }

    free(filaBuffer);
    fclose(archivo);
    return 1;
}

// [APRENDER] LiberarImagen: inversa de CrearImagen. Se libera en ORDEN INVERSO a como se
// [APRENDER] reservo: filas -> arreglo de punteros -> struct. Si se liberara primero el
// [APRENDER] arreglo se perderian las direcciones de las filas (memory leak).
void LiberarImagen(sImagen *imagen)
{
    if(imagen == NULL)
        return;

    if(imagen->pixeles != NULL)
    {
        // [APRENDER] Primero cada fila...
        for(int i=0; i<imagen->alto; i++)
        {
            free(imagen->pixeles[i]);
        }

        // [APRENDER] ...despues el arreglo de punteros (ya sin filas colgando)...
        free(imagen->pixeles);
    }

    // [APRENDER] ...y por ultimo el struct.
    free(imagen);
}

// [APRENDER] CargarImagen: funcion 'todo en uno' que van a usar los demas:
// [APRENDER] LeerBMP (encabezado) -> CrearImagen (matriz vacia) -> LeerPixeles (llenarla).
// [APRENDER] Devuelve la imagen lista o NULL si algo fallo.
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

// [APRENDER] ImprimirInfoImagen: muestra ancho x alto. Util para probar.
void ImprimirInfoImagen(const sImagen *imagen)
{
    if(imagen == NULL)
    {
        printf("\nImagen: NULL.");
        return;
    }

    printf("\nImagen: %dx%d (%d pixeles)", imagen->ancho, imagen->alto, imagen->ancho * imagen->alto);
}

// [APRENDER] CopiarImagen: crea una imagen NUEVA con el mismo contenido (copia profunda:
// [APRENDER] tiene memoria propia). Casi todos los filtros arrancan con esto para no
// [APRENDER] modificar la original.
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
            // [APRENDER] Asignar un sPixel copia sus 3 bytes (b,g,r). Son VALORES, no se comparte
            // [APRENDER] memoria: modificar la copia no toca la original.
            copia->pixeles[i][j] = original->pixeles[i][j];
        }
    }

    return copia;
}

// [APRENDER] ArmarEncabezado: construye el encabezado de una imagen NUEVA a partir de su
// [APRENDER] ancho/alto. Se recalcula siempre porque filtros como recortar, achicar o
// [APRENDER] rotar cambian el tamanio.
void ArmarEncabezado(const sImagen *imagen, sEncabezadoBMP *enc)
{
    if(imagen == NULL || enc == NULL)
        return;

    // [APRENDER] memset pone todos los bytes del struct en 0 para no dejar basura.
    memset(enc, 0, sizeof(sEncabezadoBMP));

    int bytesPorFila = CalcularBytesPorFila(imagen->ancho);
    unsigned int tamanioImagen = bytesPorFila * imagen->alto;
    unsigned int tamanioArchivo = 54 + tamanioImagen;

    enc->tipo[0] = 'B';
    enc->tipo[1] = 'M';
    enc->tamanioArchivo = tamanioArchivo;
    enc->reservado1 = 0;
    enc->reservado2 = 0;
    // [APRENDER] 54 = 14 bytes de cabecera de archivo + 40 de cabecera de imagen. Los pixeles
    // [APRENDER] empiezan justo despues (el original tenia 138; el nuestro es mas chico).
    enc->inicioDatos = 54;
    enc->tamanioCabecera = 40;
    enc->ancho = imagen->ancho;
    enc->alto = imagen->alto;
    enc->planos = 1;
    enc->bitsPorPixel = 24;
    enc->compresion = 0;
    enc->tamanioImagen = tamanioImagen;
    // [APRENDER] 2835 pixeles por metro = unos 72 dpi. Valor tipico; no afecta a la imagen.
    enc->resolucionH = 2835;
    enc->resolucionV = 2835;
    enc->coloresUsados = 0;
    enc->coloresImportantes = 0;
}

// [APRENDER] EscribirEncabezado: inversa de LeerEncabezado. fwrite campo por campo, en el
// [APRENDER] orden del formato BMP.
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

// [APRENDER] EscribirPixeles: inversa de LeerPixeles. Arma cada fila en un buffer (b,g,r
// [APRENDER] de cada pixel + padding en cero) y la escribe de una vez.
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
        // [APRENDER] Al reves que al leer: la primera fila que se escribe es la de ABAJO de la
        // [APRENDER] foto, que en la matriz es pixeles[alto-1].
        int filaOrigen = imagen->alto-1-i;
        for(int j=0; j<imagen->ancho; j++)
        {
            filaBuffer[j*3+0] = imagen->pixeles[filaOrigen][j].b;
            filaBuffer[j*3+1] = imagen->pixeles[filaOrigen][j].g;
            filaBuffer[j*3+2] = imagen->pixeles[filaOrigen][j].r;
        }

        // [APRENDER] Relleno de ceros hasta completar un multiplo de 4 bytes (lo exige el formato).
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

// [APRENDER] EscribirBMP: funcion 'todo en uno' de SALIDA: crea el archivo, escribe
// [APRENDER] encabezado y pixeles, y lo cierra. Los filtros no escriben archivos: solo
// [APRENDER] devuelven una imagen y esta funcion la guarda.
int EscribirBMP(const char *nombreArchivo, const sImagen *imagen)
{
    if(imagen == NULL)
    {
        printf("\nError: imagen NULL");
        return 0;
    }

    // [APRENDER] 'wb' = write binary: crea el archivo (si existia lo pisa). Se crea en el
    // [APRENDER] directorio desde donde se ejecuta el programa, que es lo que pide el TP.
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

