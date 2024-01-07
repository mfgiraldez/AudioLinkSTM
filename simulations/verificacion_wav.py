# -*- coding: utf-8 -*-
"""
Created on Sat Jan  6 13:32:42 2024

@author:

Representamos la señal de audio almacenada en el archivo de audio .wav
"""

import numpy as np
import matplotlib.pyplot as plt
import wave

# Abrimos el archivo de audio
audio = wave.open("mensajeModulado.wav", "r")

# Obtenemos los datos del audio
frames = audio.readframes(-1)
signal = np.frombuffer(frames, dtype="int16")

# Leemos las posiciones pares de signal
signal = signal[::2]

# Obtenemos los parámetros del audio
fs = audio.getframerate()
time = np.linspace(0, len(signal)/fs, num=len(signal))

# Cerramos el archivo de audio
audio.close()

# Graficamos la señal de audio
plt.figure()
plt.title("Señal de audio")
plt.xlabel("Tiempo [s]")
plt.ylabel("Amplitud")
plt.plot(time, signal)
plt.show()