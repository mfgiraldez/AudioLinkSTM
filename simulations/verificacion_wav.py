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
audio = wave.open("kirbyModuladoDoblePeriodo.wav", "r")

# Obtenemos los datos del audio
frames = audio.readframes(-1)
signal = np.frombuffer(frames, dtype="int16")

# Leemos las posiciones pares de signal (solo el canal 1)
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

# Coeficientes del filtro con frecuencia central f0 y ancho de banda 3 kHz, con
# una caída de 2 kHz (Gmax = 1.035)
h0 = [
    -0.000311341691536864,
    -0.000314904795981970,
    0.000508327471209295,
    0.001886713825960096,
    0.002669278280304561,
    0.001628902830305447,
    -0.001285223879872146,
    -0.004212533294060990,
    -0.003964774370143720,
    0.002107278500657691,
    -0.002546211988778623,
    0.042125896162662775,
    0.075005720377210566,
    0.063022777008648959,
    0.001168622691895534,
    -0.077692228319504730,
    -0.121860302795318648,
    -0.097149587276850802,
    -0.014025157134872221,
    0.076045894155919530,
    0.114385708484295065,
    0.076045894155918003,
    -0.014025157134872086,
    -0.097149587276850857,
    -0.121860302795318842,
    -0.077692228319504536,
    0.001168622691895495,
    0.063022777008649139,
    0.075005720377210594,
    0.042125896162662685,
    -0.002546211988778749,
    0.002107278500657585,
    -0.003964774370143736,
    -0.004212533294060951,
    -0.001285223879872101,
    0.001628902830305452,
    0.002669278280304540,
    0.001886713825960078,
    0.000508327471209286,
    -0.000314904795981983,
    -0.000311341691536884,
]

# Coeficientes del filtro con frecuencia central f1 y ancho de banda 4 kHz
# (Gmax = 1.156)
h1 = [
    0.000748334746194504,
    -0.000000000000000003,
    -0.002350362931001288,
    0.000000000000000005,
    0.004358176149758669,
    -0.000000000000000020,
    -0.006737982136279914,
    0.000000000000000018,
    0.009327852951630281,
    -0.000000000000000015,
    -0.037140910102565948,
    -0.000033058508785831,
    0.076391418046722726,
    -0.000009196971980083,
    -0.104252824980979569,
    0.000082940104585621,
    0.125391365825438861,
    -0.000230135089701787,
    -0.138773826528781424,
    0.000841885548709210,
    0.144772647754071920,
    0.000841885548709205,
    -0.138773826528779842,
    -0.000230135089701790,
    0.125391365825438805,
    0.000082940104585621,
    -0.104252824980979375,
    -0.000009196971980086,
    0.076391418046722739,
    -0.000033058508785830,
    -0.037140910102566094,
    -0.000000000000000014,
    0.009327852951630248,
    0.000000000000000014,
    -0.006737982136279903,
    -0.000000000000000016,
    0.004358176149758612,
    0.000000000000000008,
    -0.002350362931001275,
    -0.000000000000000003,
    0.000748334746194465,
]

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
            
fir0 = FIR_Filter(h0)
fir1 = FIR_Filter(h1)

salida_fir0 = np.array([])
salida_fir1 = np.array([])

for sample in signal:
    # Actualizamos el valor de los dos filtros
    fir0.FIRFilterUpdate(sample)
    fir1.FIRFilterUpdate(sample)
    
    salida_fir0 = np.append(salida_fir0,fir0.out)
    salida_fir1 = np.append(salida_fir1,fir1.out)

# Coeficientes del filtro paso de baja del detector de envolvente
h_env = [
    0.045066907137521421,
    0.045820575643761983,
    0.046501173159741213,
    0.047106642713001412,
    0.047635151455958924,
    0.048085097286934234,
    0.048455114642117027,
    0.048744079435647437,
    0.048951113128342105,
    0.049075585909023091,
    0.049117118975902395,
    0.049075585909023091,
    0.048951113128342105,
    0.048744079435647437,
    0.048455114642117027,
    0.048085097286934234,
    0.047635151455958924,
    0.047106642713001412,
    0.046501173159741213,
    0.045820575643761983,
    0.045066907137521421,
]

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
        
class EnvelopeDetectorAlternativo:
    def __init__(self,coef,fall,tol):
        
        self.LP_fir_filter = FIR_Filter(coef)
        
        self.out = 0
        self.fall = fall
        self.tol = tol
    
    def clear(self):
        self.LP_fir_filter.FIRFilterClear()
        
        self.out = 0
    
    def update(self,envelopeInput):
        
        # Tomamos valor absoluto a la muestra recibida
        sample = np.abs(envelopeInput)
        
        if self.out < sample:
            self.out = sample
        else:
            if self.out > self.tol:
                self.out /= self.fall
        
      
env0 = EnvelopeDetector(h_env)
env1 = EnvelopeDetector(h_env)
# env0 = EnvelopeDetectorAlternativo(h_env,1.15,3)
# env1 = EnvelopeDetectorAlternativo(h_env,1.15,3)

salida_env0 = np.array([])
salida_env1 = np.array([])

for sampleIndex in range(np.size(signal)):
    # Actualizamos el valor de los dos filtros
    env0.update(salida_fir0[sampleIndex])
    env1.update(salida_fir1[sampleIndex])
    
    salida_env0 = np.append(salida_env0,env0.out)
    salida_env1 = np.append(salida_env1,env1.out)
    
fig, axes = plt.subplots(3,1,figsize=(30,16))
axes[0].set_title("ENV0 OUTPUT (tx a 5kHz)")
axes[0].plot(time,salida_fir0,time,salida_env0)
axes[1].set_title("ENV1 OUTPUT (tx a 10kHz)")
axes[1].plot(time,salida_fir1,time,salida_env1)
axes[2].set_title("ENV0 y ENV1 outputs")
axes[2].plot(time,salida_env0,time,salida_env1)

# Se lleva a cabo la demodulación de la señal
#state => ["START","STOP","DATA"]
state = "SILENCE"
samplesPerBit = 32

currentBit = 0
diff = 0

# Se escriben los resultados en un fichero
file = open('mensajeDemodulado.txt','w')

for sampleIndex in range(len(salida_env0)):
    if state == "SILENCE":
        if salida_env1[sampleIndex] > 8000:
            state = "STOP"
    elif state == "STOP":
        if salida_env1[sampleIndex] < salida_env0[sampleIndex]:
            state = "START"
            # Se lleva a cabo el sincronismo
            lastBit = sampleIndex
        elif salida_env1[sampleIndex] < 100:
            state = "SILENCE"
    elif state == "START":
        diff = (sampleIndex - lastBit)
        if diff == samplesPerBit:
            state = "DATA"
            lastBit = sampleIndex
    elif state == "DATA":
        diff = (sampleIndex - lastBit)
        if currentBit == 0:
            if diff == samplesPerBit/2:
                lastBit = sampleIndex
                file.write(str(1 if salida_env1[sampleIndex] > salida_env0[sampleIndex] else 0))
                currentBit += 1
        else:
            if diff == samplesPerBit:
                if currentBit == 8:
                    # Recibimos la paridad
                    lastBit = sampleIndex
                    #file.write(" Parity -> "+str(1 if salida_env1[sampleIndex] > salida_env0[sampleIndex] else 0)+'\n')
                    file.write("\n")
                    currentBit += 1
                elif currentBit == 9:
                    currentBit = 0
                    state = "STOP"
                else:
                    lastBit = sampleIndex
                    file.write(str(1 if salida_env1[sampleIndex] > salida_env0[sampleIndex] else 0))
                    currentBit += 1

file.close()