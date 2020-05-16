from time import sleep
#path = 'C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi'
path = '/home/pi/Documents/GitHub/neopixelpi'
import csv
file = "concat.csv"
rows = []
with open(path + "/" + file, 'r', newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    next(reader)
    for row in reader:
        rows.append(row)

data = [float(row[3]) for row in rows]
dates = [row[0] for row in rows]

import serial
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)

def bleach(dhw):
    if dhw <= 1:
        return 0.2*dhw
    elif dhw <= 4:
        return 0.2 + 0.2*(dhw-1)
    elif dhw <= 8:
        return 0.8 + (dhw-4)*0.05
    else:
        return 1

index = 0
while True:
    failed = False
    dhw = data[index]
    bleached = bleach(dhw)
    health = int((1.0-bleached)*255.0) 
    index = (index + 1)%len(data)
    try:
        #ser.write(str.encode(chr(char)))
        ser.write(bytes([health]))
    except:
        failed = True
    if not failed:
        pass

    print(dates[index] + " " + str(dhw))
    # 5 days per second, 73 seconds per year, 42 minutes for 35 years
    #sleep(.2)
    sleep(0.05) #20 times faster, roughly 2 minutes for 35 years?  probably doesn't go that fast though
