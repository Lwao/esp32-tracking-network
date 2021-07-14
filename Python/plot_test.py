import matplotlib.pyplot as plt
import numpy as np  
import math
# import time

def trace(rx, ry, fig):
    d = 1 # distance between each fixed node [m]
    x = [0, d, d/2] # x-coordinate of each fixed node [m]
    y = [0 , 0, d*math.sqrt(3)/2] # y-coordinate of each fixed node [m]
    colors = ['gold', 'blue', 'red'] # colors to plot

    # plot the fixed nodes in its places
    ax = fig.gca()
    ax.cla()
    ax.set_xlim((-0.2, 1.2))
    ax.set_ylim((-0.2, 1.1))
    
    for i in range(3):
        ax.scatter(x[i], y[i], s=200, marker='o', c=colors[i])
        ax.add_patch(plt.Circle((x[i],y[i]), math.sqrt((rx-x[i])**2+(ry-y[i])**2), 
                                color=colors[i], ls = '--', fill=False, clip_on=True))
    # plot the new position for the movable node
    ax.scatter(rx, ry, s=200, marker='o', c='black')    
    return fig

d = 1
fig = plt.gcf()
for i in range(1):
    rx, ry = (np.random.rand(1,1)*d,np.random.rand(1,1)*d*math.sqrt(3)/2)
    fig = trace(rx,ry,fig)



msg_format = bytes('(-120,-121,-122)','utf-8')
msg_string = msg_format.decode('utf-8')[1:-1].split(',')
# rx, ry = [float(x) for x in msg_string]
rssiAP1, rssiAP2, rssiAP3 = [int(x) for x in msg_string]
print(rssiAP1, rssiAP2, rssiAP3)

