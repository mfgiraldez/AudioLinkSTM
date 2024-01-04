#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdint.h>

#define FIR_FILTER_LENGTH  16

typedef struct {
    float buf[FIR_FILTER_LENGTH];
    uint8_t bufIndex;
    float out;        
} FirFilter;

void FirFilter_Init(FirFilter *fir);
float FirFilter_Update(FirFilter *fir, float inp);

#endif // FIR_FILTER_H

