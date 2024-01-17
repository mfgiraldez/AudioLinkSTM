#include "criptography.h"

void SeqGenerator_Init(PsRandomSeqGenerator *seqGen, uint8_t key)
{
	// Se inicializa el valor del registro con la clave compartida
	seqGen -> reg = key;

	// Se pone en la salida el bit más significativo del registro
	seqGen -> out = (seqGen -> reg) >> 7;
}

void SeqGenerator_Update(PsRandomSeqGenerator *seqGen)
{
	// Se hace un desplazamiento hacia la derecha del registro, y
	// el bit más significativo es el resultado de la xor entre el
	// bit 4 y el 2.
	seqGen -> reg = (seqGen -> reg) >> 1;

	uint8_t segundoBit = ((seqGen -> reg) >> 1) & 0b00000001;
	uint8_t cuartoBit = ((seqGen -> reg) >> 3) & 0b00000001;
	seqGen -> reg |= cuartoBit^segundoBit;

	// Se actualiza la salida
	seqGen -> out = (seqGen -> reg) >> 7;
}
