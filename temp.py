
import serial
import time
import pandas as pd
import math
import matplotlib.pyplot as plt
import numpy as np
ser = serial.Serial('COM3', 9600)
length=0
file = open("exp_name.txt", "w")
data_ar1=[]
data_ar2=[]

while len(data_ar1)<500:
    print(len(data_ar1))
    data = ser.readline().decode('ascii')
    print(data[4:])
    if 't_1' in data:
        data_ar1.append(float(data[4:]))
    else:
        data_ar2.append(float(data[4:]))
data_ar1=[(data_ar1[i]+data_ar1[i+1])/2 for i in range(len(data_ar1)-1)]
plt.plot(data_ar1)
plt.show()

