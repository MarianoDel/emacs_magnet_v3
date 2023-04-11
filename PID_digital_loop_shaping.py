# -*- coding: utf-8 -*-
#usar python3
import numpy as np
import matplotlib.pyplot as plt

from scipy import signal

"""
        PID digital for loop shaping controller
        phase vector is answerig at nan

"""

############################################
# Which graph to show... for speed reasons #
############################################
Show_Digital_Bode = True


# Origin Pole and Two zeros (PID theory implementation)

# from the algorithm backwards
#uk = uk-1 + k1 ek + k2 ek-1 + k3 ek-2
#Uz/Ez = (b0 + b1 z-1 + b2 z-2) / (1 - z-1)
#b0 = kp + kd + ki
#b1 = -kp - 2kd
#b2 = kd
#a0 = 1
#a1 = -1

fs = 7000
# kp = 0.0
kp = 1.0
# kd = 0.0
kd = 0.007*fs
ki = 0.0
# ki = 10.0/fs

ki_eff = ki*fs
kd_eff = kd/fs
print(f'kd eff (kd/fs): {kd_eff} ki eff (ki*fs): {ki_eff}')
print()

if ki > 0.0:
    print(f'ki gain at w=1: {20*np.log10(ki_eff)}dB')

if kp > 0.0:
    print(f'kp gain: {20*np.log10(kp)}dB')

if kd > 0.0:
    print(f'kd gain at w=1: {20*np.log10(kd_eff)}dB')

print()
if ki > 0.0 and kp > 0.0:
    print(f'zero from ki kp: {ki_eff/(kp * 2 * np.pi)}Hz')

if kd > 0.0 and kp > 0.0:
    print(f'zero from kp kd: {(kp)/(kd_eff * 2 * np.pi)}Hz')

print()
b = [kp + kd + ki, -kp - 2*kd, kd]     #del spice
a = [1, -1]

print ("b vector values")
print (b)
print ("a vector values")
print (a)

print ("Digital PID:")
td = 1/fs
pid_one_pole = signal.TransferFunction(b, a, dt=td)
print (pid_one_pole)

if Show_Digital_Bode == True:
    w, mag_one_pole, phase_one_pole = signal.dbode(pid_one_pole, n = 10000)
    # w, mag_one_pole, phase_one_pole = signal.dbode(pid_one_pole, n = 100000)    
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_one_pole, 'b')
    ax1.set_title(f'PID Digital')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_one_pole, 'b')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()


