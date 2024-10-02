# Program written on 11/01/2014 by Team 4 (Ram Rohan Dantu and Prasanna Subburaj)
# Copyrights belong to Microprocessor Systems CE6302 University of Texas Dallas

import serial,time
from array import *
import os

#initialize variables
numParam = 0
numBodies = 0
def num_Width(numPar,numBod):
    return numParam / numBodies     

M = array('f',[])               #array of masses
Px = array('f',[])              #array of Position x
Py = array('f',[])              #array of Position y
Vx = array('f',[])              #array of Velocity x
Vy = array('f',[])              #array of Velocity y
#os.chdir(r'C:\Users\Rohan\Downloads\MP\reference materials\nbody_final')
#os.listdir()
file_name = r'C:\Users\Rohan\Downloads\MP\reference materials\nbody_final\nbod_init.txt.txt'

#open file
fi = open(file_name,"r")

#iterate through file
counta = -1
for line in fi:
    if counta == -1:
        numParam = int(line)
    if counta == 0:
        numBodies = int(line)
        numWidth = num_Width(numParam,numBodies)
    if counta > 0:
        if counta%numWidth == 1:
            M.append(float(line))
        if counta%numWidth == 2:
            Px.append(float(line))
        if counta%numWidth == 3:
            Py.append(float(line))
        if counta%numWidth == 4:
            Vx.append(float(line))
        if counta%numWidth == 0:
            Vy.append(float(line))
        
    counta += 1

print M
print Px
print Py
print Vx
print Vy

sendData = str(numParam) + ':'
print type(sendData)
print sendData



cortex = serial.Serial('COM4', 115200, timeout=.1)
time.sleep(1)
cortex.write(sendData)
#cortex.write("Hello Cortex M4 ")
#cortex.write("SENDDATA")
while True:
    data = cortex.readline()
    if data:
        print data
