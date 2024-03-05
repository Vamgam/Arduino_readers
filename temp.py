import serial
import time
import pandas as pd
import math
import matplotlib.pyplot as plt
import numpy as np
from time import sleep
ser = serial.Serial('COM5', 9600)
sleep(6)

ser.write(b'1')


length=0
plt.ion()
data_ar1=[]
data_ar2=[]
data_ar3=[]
file = open("exp_name.txt", "w")
first=time.time()



x1=np.linspace(0,len(data_ar1),num=len(data_ar1))
        
graph1 = plt.plot(x1,data_ar1,label='Планшет')[0]
x2=np.linspace(0,len(data_ar2),num=len(data_ar2))
        
graph2 = plt.plot(x2,data_ar2,label='Столки')[0]
x3=np.linspace(0,len(data_ar3),num=len(data_ar3))
graph3 = plt.plot(x3,data_ar3,label='Крышка')[0]

data_mean=[]
plt.grid()
plt.axhline(y=40)
plt.axhline(y=30)
plt.axhline(y=35)
plt.xlabel('Time')
plt.ylabel("T,^\circ C")
plt.pause(0.25)
sleep(5)

while len(data_ar1)<5600:
    data = ser.readline().decode('ascii')
    second=time.time()
    print(data)
    if 'plate2' in data:
        st=data.find('plate2')
        data_ar1.append(float(data[st+7:st+13]))
    if 'table' in data:
        st =data.find('table')
        data_ar2.append(float(data[st+6:st+12]))
    if 'lid2' in data :
        st=data.find('lid2')
        data_ar3.append(float(data[st+5:st+11]))
    
    
    
    
    graph1.remove()
    graph2.remove()
    graph3.remove()
    
    x2=np.linspace(0,len(data_ar2),num=len(data_ar2))
        
    graph2 = plt.plot(x2,data_ar2,color='blue',label='Столки')[0]
    x3=np.linspace(0,len(data_ar3),num=len(data_ar3))
        
    graph3 = plt.plot(x3,data_ar3,color='green',label='Крышка')[0]
    x1=np.linspace(0,len(data_ar1),num=len(data_ar1))
        
    graph1 = plt.plot(x1,data_ar1,color='red',label='Планшет')[0]
    plt.legend()

    
    
    plt.pause(0.25)
file.close()
plt.savefig(str(time.ctime(second))+'.png')
        