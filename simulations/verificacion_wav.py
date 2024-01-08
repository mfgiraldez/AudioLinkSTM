# -*- coding: utf-8 -*-
"""
Created on Sat Jan  6 13:32:42 2024

@author:

Representamos la señal de audio almacenada en el archivo de audio .wav
"""

import numpy as np
import matplotlib.pyplot as plt
import wave


#######################################################    DEFINICIÓN DE CONSTANTES    #######################################################
fs = 44100 # Frecuencia de muestreo estándar
frec0 = 5512.5 # Frecuencia de la senoidal para el cero
frec1 = 11025 # Frecuencia de la senoidal para el uno

# Coeficientes para el filtro FIR de la señal de frec0
h0 = [
    -0.000980477040514116, -0.001348321762157171, -0.000267136325661726, 0.001720389878516968, 0.002864041673378604,
    0.001771748891884235, -0.000850186978892793, -0.001957864275777040, 0.001931098343351606, 0.011843973186731332,
    -0.006150135647194379, 0.026502399477765837, 0.063602547479429505, 0.061850666732343948, -0.000162774416567133,
    -0.093434243440317977, -0.151419852563490887, -0.120033643199484980, -0.005541099365544412, 0.121864914018222983,
    0.176387910667957948, 0.121864914018221346, -0.005541099365544274, -0.120033643199485146, -0.151419852563491081,
    -0.093434243440317782, -0.000162774416567149, 0.061850666732344267, 0.063602547479429547, 0.026502399477765816,
    -0.006150135647194610, 0.011843973186731190, 0.001931098343351608, -0.001957864275777006, -0.000850186978892762,
    0.001771748891884226, 0.002864041673378578, 0.001720389878516965, -0.000267136325661727, -0.001348321762157179,
    -0.000980477040514135
    ]

# Coeficientes para el filtro FIR de la señal de frec1
h1 = [
    0.000089591477140310, -0.000000000000000004, -0.001527536797770408, 0.000000000000000001, 0.003182626981371688,
    -0.000000000000000006, -0.004585822760663518, 0.000000000000000004, 0.004508526276378652, 0.000000000000000005,
    -0.002868111045641972, 0.001019048419017586, 0.062399838968987167, -0.000642975912820564, -0.112379338985974797,
    -0.000318491606706909, 0.154486279871045740, 0.002394127098149421, -0.182521312534043911, -0.010501998128006815,
    0.174531097359076681, -0.010501998128006760, -0.182521312534042440, 0.002394127098149439, 0.154486279871045656,
    -0.000318491606706904, -0.112379338985974853, -0.000642975912820569, 0.062399838968987167, 0.001019048419017585
    ]

# Coeficientes para el filtro paso baja del detector de envolvente
h_env = [
    0.045066907137521421, 0.045820575643761983, 0.046501173159741213, 0.047106642713001412, 0.047635151455958924,
    0.048085097286934234, 0.048455114642117027, 0.048744079435647437, 0.048951113128342105, 0.049075585909023091,
    0.049117118975902395, 0.049075585909023091, 0.048951113128342105, 0.048744079435647437, 0.048455114642117027,
    0.048085097286934234, 0.047635151455958924, 0.047106642713001412, 0.046501173159741213, 0.045820575643761983,
    0.045066907137521421
]

#######################################################    DEFINICIÓN DE CLASES    #######################################################

# Clase que implementa un filtro FIR
class FIR_Filter:
    def __init__(self, coefs):
        self.h = coefs # Coeficientes del filtro
        
        # Atributos asociados al buffer circular
        self.buf = [0]*len(self.h)
        self.bufIndex = 0 
        
        # Salida actual del filtro
        self.out = 0
    
    def FIRFilterClear(self):
        self.buf = [0]*len(self.h)
        self.bufIndex = 0
        self.out = 0
    
    def FIRFilterUpdate(self, filterInput):
        
        # Metemos la última muestra en el buffer circular
        self.buf[self.bufIndex] = filterInput
        
        # Incrementamos el índice
        self.bufIndex += 1
        
        if self.bufIndex == len(self.buf):
            self.bufIndex = 0
        
        # Computamos el nuevo valor de la salida
        self.out = 0
        
        sumIndex = self.bufIndex
        
        for n in range(len(self.buf)):
            if sumIndex > 0:
                sumIndex -= 1
            else:
                sumIndex = len(self.buf)-1
                
            self.out += self.h[n]*self.buf[sumIndex]


# Clase que implementa un detector de envolvente
class EnvelopeDetector:
    def __init__(self,coef):
        
        self.LP_fir_filter = FIR_Filter(coef)
        
        self.out = 0
    
    def clear(self):
        self.LP_fir_filter.FIRFilterClear()
        
        self.out = 0
    
    def update(self,envelopeInput):
        
        # Tomamos valor absoluto a la muestra recibida
        sample = np.abs(envelopeInput)
        
        self.LP_fir_filter.FIRFilterUpdate(sample)
        
        self.out = self.LP_fir_filter.out
        
                    #######################################################    CODIGO    ######################################################
            
# Obtenemos los instantes en los que se debe evaluar el seno a cada frecuencia
t0 = np.arange(0, 1/frec0, 1/fs)
t1 = np.arange(0, 1/frec1, 1/fs)
signal0 = np.sin(2*np.pi*frec0*t0)
signal1 = np.sin(2*np.pi*frec1*t1)

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


# Creamos la seal de entrada, que en este caso sera "signal"
inputSignal = signal
inputTime = np.arange(0, np.size(inputSignal)/fs, 1/fs)

# Reiniciamos los filtros
fir0 = FIR_Filter(h0)
fir1 = FIR_Filter(h1)
fir0.FIRFilterClear()
fir1.FIRFilterClear()

salida_fir0 = np.array([])
salida_fir1 = np.array([])

for sample in inputSignal:
    # Actualizamos el valor de los dos filtros
    fir0.FIRFilterUpdate(sample)
    fir1.FIRFilterUpdate(sample)
    
    salida_fir0 = np.append(salida_fir0,fir0.out)
    salida_fir1 = np.append(salida_fir1,fir1.out)

fig, axes = plt.subplots(3,1,figsize=(30,16))
axes[0].set_title("Input signal")
axes[0].plot(inputTime,inputSignal)
axes[1].set_title("FIR0 OUTPUT (fc=5kHz)")
axes[1].plot(inputTime,salida_fir0)
axes[2].set_title("FIR1 OUTPUT (fc=10kHz)")
axes[2].plot(inputTime,salida_fir1)


# Aplicamos el detector de envolvente.
env0 = EnvelopeDetector(h_env)
env1 = EnvelopeDetector(h_env)

salida_env0 = np.array([])
salida_env1 = np.array([])

for sampleIndex in range(np.size(inputSignal)):
    # Actualizamos el valor de los dos filtros
    env0.update(salida_fir0[sampleIndex])
    env1.update(salida_fir1[sampleIndex])
    
    salida_env0 = np.append(salida_env0,env0.out)
    salida_env1 = np.append(salida_env1,env1.out)
    
fig, axes = plt.subplots(4,1,figsize=(30,16))

axes[0].set_title("ORIGINAL SIGNAL")
axes[0].plot(inputTime,inputSignal)
axes[1].set_title("ENV0 OUTPUT (tx a f0)")
axes[1].plot(inputTime,salida_fir0,inputTime,salida_env0)
axes[2].set_title("ENV1 OUTPUT (tx a f1)")
axes[2].plot(inputTime,salida_fir1,inputTime,salida_env1)
axes[3].set_title("ENV0 y ENV1 outputs")
axes[3].plot(inputTime,salida_env0,inputTime,salida_env1)


