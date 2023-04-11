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
        Control to Output Plant Analysis 

        Using Reduce model for PID controller

"""

############################################
# Which graph to show... for speed reasons #
############################################
Bode_Plant_Analog = False
Bode_Plant_Digital = True
Poles_Zeros_RM_Digital = False

Bode_PID_Digital = True
Poles_Zeros_PID_Digital = False

Bode_PID_Plant_Digital = True
Poles_Zeros_PID_Plant_Digital = False

Step_Plant_Digital = False
Step_Plant_Recursive_Digital = False

Step_PreFilter_Plant_Digital = False
Step_PreFilter_Plant_Digital_Recursive_Digital = False

Bode_PreFilter_Plant_Original_Digital = True

#TF without constant
s = Symbol('s')

# Typical Magnet Antenna
Rantenna = 12.27
Lantenna = 87.9e-3

# Sense filters
Rsense = 0.055
R1 = 180
C1 = 1e-9
R2 = 1800
C2 = 56e-9

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


#######################################
# Confirm some things on analog parts #
#######################################
filter_rm_TF = sympy_to_lti(Filter_out_rm_sim)
if Bode_Plant_Analog == True:
    wfreq = np.arange(2*np.pi, 2*np.pi*100000, 1)
    w, mag_r, phase_r = bode(filter_rm_TF, wfreq)

    fig, (ax1, ax2) = plt.subplots(2,1)
    ax1.semilogx (w/6.28, mag_r, 'b-', linewidth="1")
    ax1.set_title(f'Input to Output Analog Vinput = {Vinput}V')

    ax2.semilogx (w/6.28, phase_r, 'b-', linewidth="1")
    ax2.set_title('Phase')

    plt.tight_layout()
    plt.show()



# if Bode_Plant_Analog == True:
#     wfreq = np.arange(2*np.pi, 2*np.pi*100000, 1)
#     w, mag_p, phase_p = bode(filter_cm_TF, wfreq)    

#     fig, (ax1, ax2) = plt.subplots(2,1)
#     ax1.semilogx (w/6.28, mag_p, 'y-', linewidth="1")
#     ax1.set_title(f'Input to Output Complete Model Vinput = {Vinput}V')

#     ax2.semilogx (w/6.28, phase_p, 'y-', linewidth="1")
#     ax2.set_title('Phase')

#     plt.tight_layout()
#     plt.show()
    


##############################################
# Convert Plant at sampling frequency by zoh #
# to keep poles and zeroes                   #
##############################################
Fsampling = 7000
Tsampling = 1 / Fsampling

filter_rm_dig_zoh_n, filter_rm_dig_zoh_d, td = cont2discrete((filter_rm_TF.num, filter_rm_TF.den), Tsampling, method='zoh')

# normalized by TransferFunction
print ()
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


#################################
# Poles and Zeros Digital Plant #
#################################
if Poles_Zeros_RM_Digital == True:
    plot_argand(filter_rm_dig_zoh)



###############
# PID Digital #
###############
# Origin Pole and Two zeros (PID theory implementation)

# from the algorithm backwards
#uk = uk-1 + k1 ek + k2 ek-1 + k3 ek-2
#Uz/Ez = (b0 + b1 z-1 + b2 z-2) / (1 - z-1)
#b0 = kp + kd + ki
#b1 = -kp - 2kd
#b2 = kd
#a0 = 1
#a1 = -1

b0 = filter_rm_TF.num[0]
a1 = filter_rm_TF.den[1]
a1_pos = -a1
gain = b0 /(1. - a1_pos)
# gain_five_perc = 19. /(gain * Vin);
gain_five_perc = 19. /gain    #Vin included

print(f"filter gain: {gain} gain for 5 perc: {gain_five_perc}")

    # gain_five_perc = gain_five_perc * 128;
    # printf(" setting pid controller\n");
    # pid_st pid_control;
    # pid_control.ki = 6;
    # pid_control.kp = (short) gain_five_perc;
    # // pid_control.kp = 40;    
    # pid_control.kd = 0;

fs = Fsampling
# kp = 0.0
# kp = 1.0
kp = gain_five_perc
kd = 0.0
# kd = 0.007*fs
ki = 0.0
# ki = 10.0/fs
ki = kp * 2 * np.pi * 20 / fs


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
pid_one_pole = TransferFunction(b, a, dt=td)
print (pid_one_pole)

if Bode_PID_Digital == True:
    w, mag_one_pole, phase_one_pole = dbode(pid_one_pole, n = 10000)
    # w, mag_one_pole, phase_one_pole = signal.dbode(pid_one_pole, n = 100000)    
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_one_pole, 'c')
    ax1.set_title(f'PID Digital')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_one_pole, 'c')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

    
#############################
# Poles & Zeros PID Digital #
#############################
if Poles_Zeros_PID_Digital == True:
    plot_argand(pid_one_pole)


##########################
# PID and Plant Combined #
##########################
pid_dig_sympy = lti_to_sympy(pid_one_pole)
plant_dig_sympy = lti_to_sympy(filter_rm_dig_zoh)

pid_plant_dig_sympy = pid_dig_sympy * plant_dig_sympy
pid_plant_dig = sympy_to_lti(pid_plant_dig_sympy)

# normalize by TF
print ()
print ("PID Plant:")
pid_plant_dig = TransferFunction(pid_plant_dig.num, pid_plant_dig.den, dt=td)
print (pid_plant_dig)


#################################
# Bode on PID and Plant Digital #
#################################
if Bode_PID_Plant_Digital == True:
    w, mag_pp, phase_pp = dbode(pid_plant_dig, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_pp, 'b')
    ax1.set_title(f'Input to Output PreFilter')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_pp, 'b')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

    
###################################
# Poles & Zeros PID Plant Digital #
###################################
if Poles_Zeros_PID_Plant_Digital == True:
    plot_argand(pre_plant_dig)

    
# ##################################
# # Step Response on Digital Plant #
# ##################################
# simulation_time = 0.1
# t = np.linspace(0, simulation_time, num=(simulation_time*Fsampling))

# if Step_Plant_Digital == True:
#     tout, yout_rm_zoh = dstep([filter_rm_dig_zoh.num, filter_rm_dig_zoh.den, td], t=t)
#     yout1 = np.transpose(yout_rm_zoh)
#     yout0 = yout1[0]
#     yout_rm_zoh = yout0[:tout.size]
    
#     fig, ax = plt.subplots()
#     ax.set_title('Step Response RM [blue] CM [yellow] Digital ZOH')
#     ax.set_ylabel('Voltage Output')
#     ax.set_xlabel('Tiempo [s]')
#     ax.grid()

#     ax.plot(tout, yout_rm_zoh, 'b')
#     # ax.set_ylim(ymin=-20, ymax=100)

#     plt.tight_layout()
#     plt.show()
    
    

# ######################################################
# #  Convert to Recursive and check again the Response #
# ######################################################
# if Step_Plant_Recursive_Digital == True:
#     # ZOH
#     b_plant = np.transpose(filter_rm_dig_zoh.num)
#     a_plant = np.transpose(filter_rm_dig_zoh.den)

#     vin_plant = np.ones(t.size)
#     vout_plant_rm_method2 = np.zeros (t.size)    
#     recur_plant = RecursiveTF(b_plant, a_plant)
#     for i in range(t.size):
#         vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])


#     fig, ax = plt.subplots()
#     ax.set_title('Step Digital Plant Recursive ZOH RM blue CM yellow')
#     ax.set_ylabel('Voltage Output')
#     ax.set_xlabel('Tiempo [s]')
#     ax.grid()
#     ax.plot(t, vout_plant_rm_method2, 'b')
#     plt.tight_layout()
#     plt.show()
    

# ############################################
# # Step Response on PreFilter Plant Digital #
# ############################################
# simulation_time = 0.1
# t = np.linspace(0, simulation_time, num=(simulation_time*Fsampling))

# if Step_PreFilter_Plant_Digital == True:
#     tout, yout_rm_zoh = dstep([pre_plant_dig.num, pre_plant_dig.den, td], t=t)
#     yout1 = np.transpose(yout_rm_zoh)
#     yout0 = yout1[0]
#     yout_rm_zoh = yout0[:tout.size]
    
#     fig, ax = plt.subplots()
#     ax.set_title('Step Response PreFilter Plant')
#     ax.set_ylabel('Voltage Output')
#     ax.set_xlabel('Tiempo [s]')
#     ax.grid()

#     ax.plot(tout, yout_rm_zoh, 'b')
#     # ax.set_ylim(ymin=-20, ymax=100)

#     plt.tight_layout()
#     plt.show()


# ######################################################
# #  Convert to Recursive and check again the Response #
# ######################################################
# if Step_PreFilter_Plant_Digital_Recursive_Digital == True:
#     # ZOH
#     b_plant = np.transpose(pre_plant_dig.num)
#     a_plant = np.transpose(pre_plant_dig.den)

#     vin_plant = np.ones(t.size)
#     vout_plant_rm_method2 = np.zeros (t.size)    
#     recur_plant = RecursiveTF(b_plant, a_plant)
#     for i in range(t.size):
#         vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])


#     fig, ax = plt.subplots()
#     ax.set_title('Step Digital Plant Recursive ZOH RM blue CM yellow')
#     ax.set_ylabel('Voltage Output')
#     ax.set_xlabel('Tiempo [s]')
#     ax.grid()
#     ax.plot(t, vout_plant_rm_method2, 'b')
#     plt.tight_layout()
#     plt.show()


# ####################################################
# # Bode on PreFilter Plant and Plant only - Digital #
# ####################################################
# if Bode_PreFilter_Plant_Original_Digital == True:
#     w, mag_pp, phase_pp = dbode(pre_plant_dig, n = 10000)
#     w, mag_r_zoh, phase_r_zoh = dbode(filter_rm_dig_zoh, n = 10000)
    
#     fig, (ax1, ax2) = plt.subplots(2,1)

#     ax1.semilogx(w/(2*np.pi), mag_pp, 'b')
#     ax1.semilogx(w/(2*np.pi), mag_r_zoh, 'y')
#     ax1.set_title(f'Input to Output PreFilter&Plant [blue] Plant [yellow]')
#     ax1.set_xlabel('Frequency [Hz]')

#     ax2.semilogx(w/(2*np.pi), phase_pp, 'b')
#     ax2.semilogx(w/(2*np.pi), phase_r_zoh, 'y')
#     ax2.set_xlabel('Frequency [Hz]')

#     plt.tight_layout()
#     plt.show()

    
