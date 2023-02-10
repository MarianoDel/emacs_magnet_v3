# -*- coding: utf-8 -*-
#usar python3
import numpy as np
from sympy import *
import matplotlib.pyplot as plt
from scipy.signal import lti, bode, lsim, dbode, zpk2tf, tf2zpk, step2, cont2discrete, dstep, freqz, freqs, dlti, TransferFunction
from tc_udemm import sympy_to_lti, lti_to_sympy, plot_argand
from pid_tf import PID_int
from recursive_tf import RecursiveTF

"""
        Magnet power output on channels from 1 to 4
        Open Loop Analysis 
        starting from control_to_output.py

        Only using Reduce Model here

"""

############################################
# Which graph to show... for speed reasons #
############################################
Bode_Plant_Digital = False
Poles_Zeros_RM_Digital = False
Bode_PID_Digital = True
Bode_Open_Loop_Digital = True
    
# Poles_Zeros_CM_Digital = False

# Step_Plant_Digital = True
# Step_Plant_Recursive_Digital = True


#TF without constant
s = Symbol('s')

# Typical Magnet Antenna
Rantenna = 11.0
Lantenna = 142e-3

# Sense filters
Rsense = 0.055

# OPAMP gain
Amp_gain = 13

# VCC for low-signal graphics
# Vinput = 1
Vinput = 192

# reduced model
#    ---L---+
#           |
#           R
#           |
#           +---+
#           |   +--
#         Rsense   x13
#           |   +--
#    -------+---+

Zant = s * Lantenna + Rantenna
# Thevenin on Rsense
Zth1 = Rsense * Zant / (Rsense + Zant)
Vth1 = Vinput * Rsense / (Rsense + Zant)
TF_rm = Vth1 * Amp_gain


print (f'Voltage Sense with Vinput = {Vinput}V:')
print (' Reduce Model:')
Filter_out_rm_sim = TF_rm.simplify()
print (Filter_out_rm_sim)

filter_rm_TF = sympy_to_lti(Filter_out_rm_sim)


##############################################
# Convert Plant at sampling frequency by zoh #
# to keep poles and zeroes                   #
##############################################
Fsampling = 1500
Tsampling = 1 / Fsampling

filter_rm_dig_zoh_n, filter_rm_dig_zoh_d, td = cont2discrete((filter_rm_TF.num, filter_rm_TF.den), Tsampling, method='zoh')

# normalized by TransferFunction
print ("Digital Filter Reduce Model Zoh:")
filter_rm_dig_zoh = TransferFunction(filter_rm_dig_zoh_n, filter_rm_dig_zoh_d, dt=td)
print (filter_rm_dig_zoh)


########################################
# Confirm some things on digital parts #
########################################
if Bode_Plant_Digital == True:
    w, mag_r_zoh, phase_r_zoh = dbode(filter_rm_dig_zoh, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_r_zoh, 'b')
    ax1.set_title(f'Input to Output Digital ZOH Vinput = {Vinput}V')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_r_zoh, 'b')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()


##################################
# Poles & Zeros of Digital Plant #
##################################
if Poles_Zeros_RM_Digital == True:
    plot_argand(filter_rm_dig_zoh)


####################################
# Digital PID from trial and error #
####################################
## values used on embedded
kp = 10
ki = 10
kd = 0
div = 128
pid = PID_int(1,1, div)
pid.changeParamsFromK(kp, ki, kd)
pid.showParamsFromK(kp, ki, kd, Fsampling)

b_float = pid.b_pid / pid.div
a_float = [1., -1.]
print(b_float)
print(a_float)
pid_dig = TransferFunction(b_float, a_float, dt=td)
print (pid_dig)

if Bode_PID_Digital == True:
    w, mag_pid, phase_pid = dbode(pid_dig, n = 10000)

    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_pid, 'c')
    ax1.set_title(f'PID Digital')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_pid, 'c')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()


# Multiply systems
plant_dig = lti_to_sympy (filter_rm_dig_zoh)
contr_dig = lti_to_sympy (pid_dig)

open_loop = plant_dig * contr_dig
open_loop = sympy_to_lti(open_loop)

# normalized by TransferFunction
open_loop_dig = TransferFunction(open_loop.num, open_loop.den, dt=td)
print ('Open Loop Digital:')
print (open_loop_dig)

if Bode_Open_Loop_Digital == True:
    w, mag, phase = dbode(open_loop_dig, n = 10000)

    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag, 'c')
    ax1.set_title(f'Open Loop Digital')
    ax1.set_xlabel('Frequency [Hz]')
    ax1.set_ybound(lower=None, upper=60)

    ax2.semilogx(w/(2*np.pi), phase, 'c')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()


