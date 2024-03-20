
import matplotlib.pyplot as plt
from numpy import linspace
from serial import Serial

from time import sleep
from time import time,ctime
import serial.tools.list_ports




ports=serial.tools.list_ports.comports()
c=str(int(input("С какой температуры вы хотите начать:"))-1)
port=0
for i in ports:
    if "CH340" in i[1]:
        port=i[0]
        break
if port==0:
    raise IOError("Arduino not found")

ser = Serial(port, 9600)
sleep(6)

ser.write(bytes(c,"utf-8"))


length=0
plt.ion()
data_ar1=[]
data_ar2=[]
data_ar3=[]
file = open("exp_name.txt", "w")




x1=linspace(0,len(data_ar1),num=len(data_ar1))

graph1 = plt.plot(x1,data_ar1,label='Планшет')[0]
x2=linspace(0,len(data_ar2),num=len(data_ar2))

graph2 = plt.plot(x2,data_ar2,label='Столки')[0]
x3=linspace(0,len(data_ar3),num=len(data_ar3))
graph3 = plt.plot(x3,data_ar3,label='Крышка')[0]


plt.grid()
plt.axhline(y=40)
plt.axhline(y=30)
plt.axhline(y=35)
plt.xlabel('Time')
plt.ylabel("T,^\circ C")
plt.pause(0.25)
sleep(5)
try:
    for i in range(4):
        data = ser.readline().decode('ascii')
        print(data)
    while len(data_ar1)<5600:
        data = ser.readline().decode('ascii')
        print(data)
        if 'PLN' in data:
            st=data.find('PLN')
            data_ar1.append(float(data[st+4:st+9]))
        if 'TBL' in data:
            st=data.find("TBL")
            data_ar2.append(float(data[st+4:st+9]))
        if 'CRS' in data:
            st=data.find('CRS')
            data_ar3.append(float(data[st+4:st+9]))
        file.write(str(ctime(time())).split(' ')[3]+' '+str(data_ar1[-1])+'\n')

        graph1.remove()
        graph2.remove()
        graph3.remove()

        x2=linspace(0,len(data_ar2),num=len(data_ar2))

        graph2 = plt.plot(x2,data_ar2,color='blue',label='Столки')[0]
        x3=linspace(0,len(data_ar3),num=len(data_ar3))

        graph3 = plt.plot(x3,data_ar3,color='green',label='Крышка')[0]
        x1=linspace(0,len(data_ar1),num=len(data_ar1))

        graph1 = plt.plot(x1,data_ar1,color='red',label='Планшет')[0]
        plt.legend()
        plt.pause(0.25)

finally :
    file.close()
    plt.savefig(str(ctime(time()).split(' ')[:3])+'.png')
 

    