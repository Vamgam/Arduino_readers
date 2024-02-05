import serial
import time
import pandas as pd
import math
import matplotlib.pyplot as plt
ser = serial.Serial('COM3', 9600)
length=0
file = open("exp_name.txt", "w")
data_ar=[]
data1=[]
first=time.time()
second=time.time()
while length<1000 :
    data = ser.readline().decode('ascii')  ## Read a line of data and decode it as an ASCII string
    second=time.time()
    data_ar.append(float(data[4:]))
    if math.isclose((second-first),0,abs_tol=0.2):
        file.write(str(time.ctime(time.time()))+' '+'%.2f'%(sum(data_ar)/len(data_ar))+'\n')
        data1.append(sum(data_ar)/len(data_ar))
        data_ar=[]
    length+=1
    print(length)
file.close()

plt.plot(data1)
plt.show()