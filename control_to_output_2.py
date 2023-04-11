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

        Using Reduce and pre distorsion on signal input

"""

############################################
# Which graph to show... for speed reasons #
############################################
Bode_Plant_Analog = False
Bode_Plant_Digital = True
Poles_Zeros_RM_Digital = False

Bode_PreFilter_Digital = True
Poles_Zeros_PreFilter_Digital = False

Bode_PreFilter_Plant_Digital = False
Poles_Zeros_PreFilter_Plant_Digital = False

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

# if Bode_Plant_Digital == True:
#     w, mag_zoh, phase_zoh = dbode(filter_opamp_dig_zoh, n = 10000)

#     fig, (ax1, ax2) = plt.subplots(2,1)

#     ax1.semilogx(w/(2*np.pi), mag_zoh, 'y')
#     ax1.set_title(f'Filter and Opamp Digital Bode ZOH Vinput = {Vinput}V')
#     ax1.set_xlabel('Frequency [Hz]')

#     ax2.semilogx(w/(2*np.pi), phase_zoh, 'y')
#     ax2.set_xlabel('Frequency [Hz]')

#     plt.tight_layout()
#     plt.show()
    

    

######################################
# Polos y Ceros de la planta Digital #
######################################
if Poles_Zeros_RM_Digital == True:
    plot_argand(filter_rm_dig_zoh)



#####################
# PreFilter Digital #
#####################
pre_dig_n = [1, -0.98]
# pre_dig_d = [1, 0, 0]
pre_dig_d = [1, -0.6]
# pre_dig_d = [1]
pre_dig_gain = (1. - 0.6)/( 1. - 0.98)
pre_dig_n = list(np.array(pre_dig_n) * pre_dig_gain)

# normalized by TransferFunction
print ()
print ("PreFilter from poles and zeros:")
pre_dig = TransferFunction(pre_dig_n, pre_dig_d, dt=td)
print (pre_dig)


#############################
# Bode on PreFilter Digital #
#############################
if Bode_PreFilter_Digital == True:
    w, mag_p, phase_p = dbode(pre_dig, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_p, 'b')
    ax1.set_title(f'Input to Output PreFilter')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_p, 'b')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

    
###################################
# Poles & Zeros PreFilter Digital #
###################################
if Poles_Zeros_PreFilter_Digital == True:
    plot_argand(pre_dig)


################################
# PreFilter and Plant Combined #
################################
pre_dig_sympy = lti_to_sympy(pre_dig)
plant_dig_sympy = lti_to_sympy(filter_rm_dig_zoh)

pre_plant_dig_sympy = pre_dig_sympy * plant_dig_sympy
pre_plant_dig = sympy_to_lti(pre_plant_dig_sympy)

# normalize by TF
print ()
print ("PreFilter Plant:")
pre_plant_dig = TransferFunction(pre_plant_dig.num, pre_plant_dig.den, dt=td)
print (pre_plant_dig)

###################################
# Bode on PreFilter Plant Digital #
###################################
if Bode_PreFilter_Plant_Digital == True:
    w, mag_pp, phase_pp = dbode(pre_plant_dig, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_pp, 'b')
    ax1.set_title(f'Input to Output PreFilter')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_pp, 'b')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

    
#########################################
# Poles & Zeros PreFilter Plant Digital #
#########################################
if Poles_Zeros_PreFilter_Plant_Digital == True:
    plot_argand(pre_plant_dig)

    
##################################
# Step Response on Digital Plant #
##################################
simulation_time = 0.1
t = np.linspace(0, simulation_time, num=(simulation_time*Fsampling))

if Step_Plant_Digital == True:
    tout, yout_rm_zoh = dstep([filter_rm_dig_zoh.num, filter_rm_dig_zoh.den, td], t=t)
    yout1 = np.transpose(yout_rm_zoh)
    yout0 = yout1[0]
    yout_rm_zoh = yout0[:tout.size]
    
    fig, ax = plt.subplots()
    ax.set_title('Step Response RM [blue] CM [yellow] Digital ZOH')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()

    ax.plot(tout, yout_rm_zoh, 'b')
    # ax.set_ylim(ymin=-20, ymax=100)

    plt.tight_layout()
    plt.show()
    
    

######################################################
#  Convert to Recursive and check again the Response #
######################################################
if Step_Plant_Recursive_Digital == True:
    # ZOH
    b_plant = np.transpose(filter_rm_dig_zoh.num)
    a_plant = np.transpose(filter_rm_dig_zoh.den)

    vin_plant = np.ones(t.size)
    vout_plant_rm_method2 = np.zeros (t.size)    
    recur_plant = RecursiveTF(b_plant, a_plant)
    for i in range(t.size):
        vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])


    fig, ax = plt.subplots()
    ax.set_title('Step Digital Plant Recursive ZOH RM blue CM yellow')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()
    ax.plot(t, vout_plant_rm_method2, 'b')
    plt.tight_layout()
    plt.show()
    

############################################
# Step Response on PreFilter Plant Digital #
############################################
simulation_time = 0.1
t = np.linspace(0, simulation_time, num=(simulation_time*Fsampling))

if Step_PreFilter_Plant_Digital == True:
    tout, yout_rm_zoh = dstep([pre_plant_dig.num, pre_plant_dig.den, td], t=t)
    yout1 = np.transpose(yout_rm_zoh)
    yout0 = yout1[0]
    yout_rm_zoh = yout0[:tout.size]
    
    fig, ax = plt.subplots()
    ax.set_title('Step Response PreFilter Plant')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()

    ax.plot(tout, yout_rm_zoh, 'b')
    # ax.set_ylim(ymin=-20, ymax=100)

    plt.tight_layout()
    plt.show()


######################################################
#  Convert to Recursive and check again the Response #
######################################################
if Step_PreFilter_Plant_Digital_Recursive_Digital == True:
    # ZOH
    b_plant = np.transpose(pre_plant_dig.num)
    a_plant = np.transpose(pre_plant_dig.den)

    vin_plant = np.ones(t.size)
    vout_plant_rm_method2 = np.zeros (t.size)    
    recur_plant = RecursiveTF(b_plant, a_plant)
    for i in range(t.size):
        vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])


    fig, ax = plt.subplots()
    ax.set_title('Step Digital Plant Recursive ZOH RM blue CM yellow')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()
    ax.plot(t, vout_plant_rm_method2, 'b')
    plt.tight_layout()
    plt.show()


####################################################
# Bode on PreFilter Plant and Plant only - Digital #
####################################################
if Bode_PreFilter_Plant_Original_Digital == True:
    w, mag_pp, phase_pp = dbode(pre_plant_dig, n = 10000)
    w, mag_r_zoh, phase_r_zoh = dbode(filter_rm_dig_zoh, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_pp, 'b')
    ax1.semilogx(w/(2*np.pi), mag_r_zoh, 'y')
    ax1.set_title(f'Input to Output PreFilter&Plant [blue] Plant [yellow]')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_pp, 'b')
    ax2.semilogx(w/(2*np.pi), phase_r_zoh, 'y')
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

    
