
import matplotlib.pyplot as plt
from numpy import linspace
from serial import Serial
from time import sleep
from datetime import datetime
#import serial.tools.list_ports
from tkinter import Tk
from tkinter.filedialog import askdirectory
import os
import numpy as np

root = Tk()
path = askdirectory(title='Select Folder') # shows dialog box and return the path
root.destroy()

file_index = 0
filename = os.path.join(path, "temp_curve")
while os.path.exists(f"{filename}{file_index}.txt"):
    file_index += 1
file = open(f"{filename}{file_index}.txt", "w")

c=str(int(input("С какой температуры вы хотите начать:"))-1)
#c = "0"

ser = Serial("COM5", 9600)
sleep(6)

ser.write(bytes(c,"utf-8"))


length=0
plt.ion()
plate_temp,lid_temp, table_temp, plate_state, lid_state =[], [], [], [], []


time_min=linspace(0,len(plate_temp),num=len(plate_temp))*0.5/60


fig = plt.figure()
ax = plt.gca()
#ax2 = ax.twinx()

graph_plate = ax.plot(time_min, plate_temp, label=f'Планшет', c = 'r')[0]
graph_table = ax.plot(time_min, table_temp, label='Столики', c = 'g')[0]
graph_lid = ax.plot(time_min, lid_temp, label='Крышка', c = 'b')[0]

# graph_plate_state = ax.plot(time_min, plate_state, '--k', label = 'plate_state')[0]
# graph_lid_state = ax.plot(time_min, lid_state, '--m', label = 'lid_state')[0]
plt.grid()
plt.xlabel('Time, min')
plt.ylabel("$T,^\circ C$")
plt.pause(0.25)
sleep(5)
try:
    for i in range(4):
        data = ser.readline().decode('ascii')
        print(data)
    while True:
        data = ser.readline().decode('ascii')
        if data == 'termination':
            break
        print(data)
        data_list = {item.split("=")[0]:float(item.split("=")[1]) for item in data.split(";")}

        plate_state.append(data_list['plate_state'])
        lid_state.append(data_list['lid_state'] + 0.05)
        plate_temp.append(data_list['plate'])
        lid_temp.append(data_list['lid'])
        table_temp.append(data_list['table'])

        
        time_min=linspace(0,len(plate_temp),num=len(plate_temp))*0.5/60

        graph_plate.set_xdata(time_min)
        graph_plate.set_ydata(plate_temp)

        graph_lid.set_xdata(time_min)
        graph_lid.set_ydata(lid_temp)

        graph_table.set_xdata(time_min)
        graph_table.set_ydata(table_temp)

        # graph_plate_state.set_xdata(time_min)
        # graph_plate_state.set_ydata(plate_state)

        # graph_lid_state.set_xdata(time_min)
        # graph_lid_state.set_ydata(lid_state)

        ax.relim()
        ax.autoscale_view()
        # ax2.relim()
        # ax2.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

        file.write(f"{datetime.now().strftime('%H:%M:%S')},{plate_temp[-1]} \n")
        #ax2.legend(loc = "lower right")
        ax.legend(loc = "lower left")
        plt.pause(0.25)


finally :
    file.close()
    plt.savefig(f"{filename}{file_index}.png")
 

    