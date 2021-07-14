import paho.mqtt.client as mqtt
import time
import matplotlib.pyplot as plt
import math


d = 1 # distance between each fixed node [m]
# set new pseudo random file output name
random_time_stamp = str(time.time()).replace('.','')
file_name = 'section_' + random_time_stamp + '.txt'


def RSSI2coordinates(rssi_vector):
    # calibration constants
    RSSI_REF1 = -43.5225
    RSSI_REF2 = -26.3252
    RSSI_REF3 = -29.7935
    PATH_LOSS = 2
    
    # radius from each node to reference node
    d_AP1 = 10**((rssi_vector[0]-RSSI_REF1)/(-10*PATH_LOSS))
    d_AP2 = 10**((rssi_vector[1]-RSSI_REF2)/(-10*PATH_LOSS))
    d_AP3 = 10**((rssi_vector[2]-RSSI_REF3)/(-10*PATH_LOSS))
    
    # coordinates for movable node
    rx = (d_AP1**2-d_AP2**2+d**2)/(2*d);
    ry = (d_AP1**2+d_AP2**2-2*d_AP3**2+d**2)/(2*math.sqrt(3)*d);
    return rx, ry

# Function to trace the nodes spatial position
def trace(rx, ry, fig):
    x = [0, d, d/2] # x-coordinate of each fixed node [m]
    y = [0 , 0, d*math.sqrt(3)/2] # y-coordinate of each fixed node [m]
    colors = ['gold', 'blue', 'red'] # colors to plot

    # plot the fixed nodes in its places
    ax = fig.gca()
    ax.cla()
    #ax.set_xlim((-0.2, 1.2))
    #ax.set_ylim((-0.2, 1.1))
    
    for i in range(3):
        ax.scatter(x[i], y[i], s=200, marker='o', c=colors[i])
        ax.add_patch(plt.Circle((x[i],y[i]), math.sqrt((rx-x[i])**2+(ry-y[i])**2), 
                                color=colors[i], ls = '--', fill=False, clip_on=True))
    # plot the new position for the movable node
    ax.scatter(rx, ry, s=200, marker='o', c='black')    
    plt.pause(0.05)
    plt.show()
    return fig

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        # print("Connected successfully")
        global connected
        connected=True
    else:
        # None
        print("Connect returned result code: " + str(rc))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Received message: " + msg.topic + " -> " + msg.payload.decode("utf-8"))
    
    # convert received message to float list/vector
    rssi_vector = []
    [rssi_vector.append(float(x)) for x in msg.payload.decode("utf-8")[1:-1].split(',')]
    print('RSSI vector: ' + str(rssi_vector))
    # call RSSI to distance conversion routine and plot coordinates
    fig = plt.gcf()
    rx, ry = RSSI2coordinates(rssi_vector)
    print('(rx,ry) = ' + '(' + str(rx) + ',' + str(ry) + ')')
    fig = trace(rx,ry,fig)
    file = open(file_name,"a") # open section file
    # append in text file in the format [d, rssi1, rssi2, rssi3, rx, ry]
    file.write(str(d) + ',' + str(rssi_vector[0]) + ',' + str(rssi_vector[1]) + ',' + 
               str(rssi_vector[2]) + ',' + str(rx) + ',' + str(ry) + '\n')
    file.close() # close section file
    

connected = False # connection status
msgReceived = False # message status

brokerAddress = "192.168.0.6" # broker address
port = 1883 # TCP/IP port
userName = "mqtt" # server username
passWord = "password" # server password

# mqtt client configuration
client = mqtt.Client("Dashboard") # create the mqtt client
client.on_connect = on_connect # set on connect callback
client.on_message = on_message # set on message callback
client.username_pw_set(userName, passWord) # set username and password
client.connect(brokerAddress, port) # connect to mosquitto broker on port 1883

#client.tls_set(tls_version=mqtt.ssl.PROTOCOL_TLS) # enable TLS

# subscribe to desired topics
client.subscribe("/RSSI_vector")

client.loop_start()

while connected!=True:
    time.sleep(0.2)
while msgReceived!=True:
    time.sleep(0.2)


client.loop_stop()


# client.loop_forever()