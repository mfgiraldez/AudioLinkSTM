#ifndef INC_CRIPTOGRAPHY_H_
#define INC_CRIPTOGRAPHY_H_

#include <stdint.h>

/*
 * Se define una estructura que utiliza un registro de 8 bits, que
 * inicialmente será definido al mismo valor inicial en transmisor
 * y receptor (será la clave compartida).
 */
typedef struct {
    uint8_t reg;
    float out;
} PsRandomSeqGenerator;

void SeqGenerator_Init(PsRandomSeqGenerator *seqGen, uint8_t key);
void SeqGenerator_Update(PsRandomSeqGenerator *seqGen);

#endif /* INC_CRIPTOGRAPHY_H_ */
