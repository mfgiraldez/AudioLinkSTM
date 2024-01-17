#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <stdint.h>

#define MAX_FIR_FILTER_LENGTH  41
#define LOW_PASS 0
#define BAND_PASS_5K 1
#define BAND_PASS_10K 2
#define LOW_PASS_LENGTH 21
#define BAND_PASS_LENGHT 41

typedef struct {
    float buf[MAX_FIR_FILTER_LENGTH];
    uint8_t bufIndex;
    float out;
    uint8_t type;
} FirFilter;

void FirFilter_Init(FirFilter *fir, uint8_t type);
float FirFilter_Update(FirFilter *fir, float inp);

#endif // FIR_FILTER_H

