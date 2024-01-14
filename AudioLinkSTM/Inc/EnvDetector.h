#ifndef ENVDETECTOR_H
#define ENVDETECTOR_H

#include <stdint.h>
#include "FirFilter.h"

typedef struct {
    float out;
    FirFilter envLPFilter;
} EnvDetector;

void EnvDetector_Init(EnvDetector *env);
float EnvDetector_Update(EnvDetector *env, float inp);

#endif // FIR_FILTER_H
