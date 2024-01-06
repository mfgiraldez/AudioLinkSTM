# -*- coding: utf-8 -*-
"""
Created on Sat Jan  6 13:32:42 2024

@author:
"""

import numpy as np

fs = 44100 # Frecuencia de muestreo estándar
frec0 = 5512.5 # Frecuencia de la senoidal para el cero

# Obtenemos los instantes en los que se debe evaluar el seno a cada frecuencia
t0 = np.arange(0, 1/frec0, 1/fs)

signal0 = np.sin(2*np.pi*frec0*t0)

# Escalamos las señales signal0 y signal1 para que estén entre -32768 y 32767
signal0 *= 32767

# Se deben cuantizar las señales en niveles de -32768 a 32767 (16 bits en complemento a 2)
signal0 = np.round(signal0.astype(int))

def twos_complement(sample):
    if sample >= 0:
        return format(sample, '016b')
    else:
        return format((abs(sample) ^ 0xFFFF) + 1, '016b')

# Pasamos ahora las muestras a binario en complemento a 2 y las imprimimos en el formato de 
# un array de C
print("int16_t sineSamples[] = {")

for i in range(len(signal0)):
    sample = signal0[i]
    # Escribimos la coma solo si la muestra no es la última de la lista
    if i != len(signal0)-1:
        print("    0b" + twos_complement(sample) + ",")
    else:
        print("    0b" + twos_complement(sample))

print("};")