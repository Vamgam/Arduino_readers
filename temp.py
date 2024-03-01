import serial
import time
import pandas as pd
import math
import matplotlib.pyplot as plt
import numpy as np
ser = serial.Serial('COM5', 9600)
length=0
plt.ion()
data_ar1=[]
data_ar2=[]
x=np.linspace(0,len(data_ar1),num=len(data_ar1))


graph = plt.plot(x,data_ar1)[0]
plt.grid()
plt.axhline(y=40)
plt.pause(0.25)
while len(data_ar1)<5600:
    data = ser.readline().decode('ascii')
    if 'PLN' in data:
        print(data)
        data_ar1.append(float(data[4:]))
        graph.remove()
        x=np.linspace(0,len(data_ar1),num=len(data_ar1))
        
        graph = plt.plot(x,data_ar1)[0]
        plt.axhline(y=30)
        plt.grid()
        plt.pause(0.25)
        