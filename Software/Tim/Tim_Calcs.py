#The purpose of this calculator is to eventually be able to simulate the system, being able to take any input and calculate
#all information that could be useful while making variables accessible
#but im overthinking everything in this so things might not make sense as i play around with stuff
import matplotlib.pyplot as plt
import numpy as np
import math
import time

#Length Values
L_tether = 1000 #Length of tether (m)
D_tether = 0.0007 #Diameter of tether (m)
Dmin_spool = 0.03 #Smallest diameter of spool (m)
Dmax_spool = 0.1 #Largest diameter of spool (m)
L_sp = 0.1 #Length of spool (m)
num_rot_spool = L_sp//D_tether #142

#Mass Values
m_string = 0.0008889 #kg/m
m_moby = 1.5 #kg
m_spool = 
g = 9.81 #m/s^2
F_drag = 33.1 #N (assumption from doc, maybe will recalculate with variable parameters)
percent_drag = 1 #what percent of drag are we using for the calc
drag = dpercent_drag*F_drag

## == LENGTH CALCULATIONS == ##
dia = np.arange(Dmin_spool + D_tether, 2*D_tether*1000 + Dmin_spool+D_tether + 2*D_tether, 2*D_tether) #the set of all diameters until 1000 layers

# Assumes that the tether is going to lie perfectly on top on the layer below
def distance(point_on_tether):
    leftover = point_on_tether
    layer = 0
    for i in range(0, len(dia)):
        layer_length = dia[i]*math.pi*num_rot_spool
        print(layer_length)
        if leftover <= layer_length:
            layer = i
            wraps = leftover//(dia[i]*math.pi)
            angle = ((leftover/(dia[i]*math.pi))%1)*2*math.pi #radians
            return dia[i], layer, wraps, angle
            break
        elif leftover > layer_length:
            leftover = leftover - layer_length
            print("This much tether remaining: " + str(leftover))

def torque(point_on_tether):
    r = distance(point_on_tether)[0]/2
    

def timing(spool_rpm, point_on_tether):
    pass

print(distance(1000))

## == TORQUE AND RPM CALCS == ##

class Motor:
    def __init__(self, rpm, torque):
        self.rpm = rpm
        self.torque = torque


