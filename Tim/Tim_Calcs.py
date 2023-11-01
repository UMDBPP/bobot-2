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

## == LENGTH CALCULATIONS == ##
points = np.arange(0, 1000 + 0.001, 0.001) #for any given point on the tether
rotation = 0 #the rotation that the tether is completing
layer = 0 #the layer that the tether is on
D_loop = Dmin_spool + D_tether #diameter of spool at a given time

# Assumes that the tether is going to lie perfectly on top on the layer below
dist_tether = []
spool = []
curr_layer = 0
print(points)
for point in points:
    tot_wraps = point/D_loop #total wraps overall
    curr_wraps = tot_wraps - curr_layer*num_rot_spool #current number of wraps on the layer
    #print("current layer wraps = " +str(curr_wraps))
    #print("total_wraps = " + str(tot_wraps))
    layers = tot_wraps//num_rot_spool #total number of layers
    #print("number of layers = " + str(layers))
    #print("current layer = " + str(curr_layer))
    if curr_wraps > num_rot_spool:
        curr_layer += 1
        D_loop = D_loop + 2*D_tether
        amp = D_loop
    elif curr_wraps <= num_rot_spool:
        amp = D_loop*math.sin(curr_wraps%1*2*math.pi)
    
    dist_tether.append(point)
    spool.append(amp)

print("The number layers on the spool is: " + str(layers))
print("Total number of wraps is: " + str(tot_wraps))

plt.figure(1)
plt.title('Distance Visualization')
plt.plot(dist_tether, spool)
plt.xlabel('Distance along tether')
plt.ylabel('Position on spool')
plt.show()

## == TORQUE AND RPM CALCS == ##

