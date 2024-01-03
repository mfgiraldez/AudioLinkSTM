#include "FirFilter.h"

static float FIR_IMPULSE_RESPONSE[FIR_FILTER_LENGTH] = () // FIR filter coefficients;

void FirFilter_Init(FirFilter *fir) {
    
    /*Limpiar el buffer del filtro*/
    for (uint8_t n = 0; n < FIR_FILTER_LENGTH; n++) {
        fir->buf[n] = 0;
    }

    /* Reseteamos elindice del buffer */
    fir -> bufIndex = 0;

    /* Limpiamos la salida del filtro */
    fir -> out = 0.0f;
}

/* 
    *fir => puntero a la estructura "fir"
    inp => ultima entrada al filtro 
*/
float FirFilter_Update(FirFilter *fir, float inp) {
    /* Se guarda laultima muestra en el buffer */
    fir->buf[fir->bufIndex] = inp;

    /* Incremento el indice dle buffer */
    fir->bufIndex++;

    /* Se compruba que el tamaño es menor que el tamaño maximo */
    if(fir->bufIndex == FIR_FILTER_LENGTH) {
        fir->bufIndex = 0;
    }

    /* A partir de aqui se computa la siguiente salida del filtro realizando la convolucion */
    /* Se empieza con la salida a cero */
    fir-> out = 0.0f;

    /* Definimos un indice para la suma de la convolucion */
    uint8_t sumIndex = fir->bufIndex;

    /* Creamos un bucle que englobe a toda la longitud del filtro (numero de coeficientes) */
    for(uint8_t n = 0; n < FIR_FILTER_LENGTH; n++) {
        /* Comprobamos que el indice sea mayor que cero y en ese caso decrementamos */
        if(sumIndex > 0) {
            sumIndex--;
        } else {
            sumIndex = FIR_FILTER_LENGTH - 1;
        }

        /* Multiplicamos la respuesta al impulso con la muestra y lo añadimos a la salida */
        fir->out += FIR_IMPULSE_RESPONSE[n] * fir->buf[sumIndex];
    }

    return fir->out;
}
