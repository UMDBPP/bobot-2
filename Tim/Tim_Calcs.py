import matplotlib.pyplot as plt
import numpy as np
import math
import time

L_tether = 1000 #Length of tether (m)
D_tether = 0.0007 #Diameter of tether (m)
Dmin_spool = 0.03 #Smallest diameter of spool (m)
Dmax_spool = 0.1 #Largest diameter of spool (m)
L_sp = 0.1 #Length of spool (m)
num_rot_spool = L_sp//D_tether
print(num_rot_spool)

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
            return layer, wraps, angle
            break
        elif leftover > layer_length:
            leftover = leftover - layer_length
            print("This much tether remaining: " + str(leftover))

def timing(spool_rpm, point_on_tether):
    pass
print(distance(1000))


#print("The number layers on the spool is: " + str(layers))
#print("Total number of wraps is: " + str(tot_wraps))
'''
plt.figure(1)
plt.title('Distance Visualization')
plt.plot(dist_tether, spool)
plt.xlabel('Distance along tether')
plt.ylabel('Position on spool')
plt.show()
'''
## == TORQUE AND RPM CALCS == ##

class Motor:
    def __init__(self, rpm, torque):
        self.rpm = rpm
        self.torque = torque


