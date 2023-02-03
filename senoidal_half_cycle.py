# -*- coding: utf-8 -*-
#usar python3
import numpy as np
import matplotlib.pyplot as plt

muestras = 256
Vmax = 1023
Voffset = 512

s_sen_half = np.zeros(muestras)
s_sen_full = np.zeros(muestras)
s_sen_rect = np.zeros(muestras)


for i in range(np.size(s_sen_half)):
    s_sen_half[i] = np.sin(np.pi*(i+1)/muestras) * Vmax
    s_sen_full[i] = np.sin(2*np.pi*(i+1)/muestras) * (Vmax - Voffset)
    s_sen_full[i] += Voffset
    # rectified sinusoidal
    s_sen_rect[i] = np.sin(2*np.pi*(i+1)/muestras) * Vmax
    if s_sen_rect[i] < 0:
        s_sen_rect[i] = 0
    


s_sen_half_int = s_sen_half.astype(int)
s_sen_full_int = s_sen_full.astype(int)
s_sen_rect_int = s_sen_rect.astype(int)


### print on C code format ###
cant_por_linea = 10

print ('--- SINUSOIDAL HALF CYCLE ---')
linea = 1
print ("{",end='')
for i in range(np.size(s_sen_half_int)):
    if i < ((linea * cant_por_linea) - 1):
        print (str(s_sen_half_int[i]) + ",",end='')
    else:
        if i == (np.size(s_sen_half_int) - 1):
            print (str(s_sen_half_int[i]),end='')
        else:                
            print (str(s_sen_half_int[i]) + ",\n",end='')
            linea += 1
        
print ("};")

print ()
print ()
print ('--- SINUSOIDAL FULL CYCLE ---')
linea = 1
print ("{",end='')
for i in range(np.size(s_sen_full_int)):
    if i < ((linea * cant_por_linea) - 1):
        print (str(s_sen_full_int[i]) + ",",end='')
    else:
        if i == (np.size(s_sen_full_int) - 1):
            print (str(s_sen_full_int[i]),end='')
        else:                
            print (str(s_sen_full_int[i]) + ",\n",end='')
            linea += 1
        
print ("};")


print ()
print ()
print ('--- SINUSOIDAL RECTIFIED ---')
linea = 1
print ("{",end='')
for i in range(np.size(s_sen_rect_int)):
    if i < ((linea * cant_por_linea) - 1):
        print (str(s_sen_rect_int[i]) + ",",end='')
    else:
        if i == (np.size(s_sen_rect_int) - 1):
            print (str(s_sen_rect_int[i]),end='')
        else:                
            print (str(s_sen_rect_int[i]) + ",\n",end='')
            linea += 1
        
print ("};")

