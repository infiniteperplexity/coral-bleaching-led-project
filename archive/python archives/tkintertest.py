import time
#import board
#import neopixel
import csv
import tkinter as tk

## import the data...right now it's DHW, but I could get temperature as well
path = 'C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/'
file = "concat.csv"
rows = []
with open(path + file, 'r', newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    header = next(reader)
    for row in reader:
        rows.append(row)

data = [float(row[3]) for row in rows]
dates = [row[0] for row in rows]

## get the board set up
#pixel_pin = board.D18
num_pixels = 109
#ORDER = neopixel.GRB
#pixels = neopixel.NeoPixel(
#    pixel_pin, num_pixels, brightness=0.2, auto_write=False, pixel_order=ORDER
#)

## bleaching function
def bleach(dhw):
    if dhw <= 1:
        return 0.2*dhw
    elif dhw <= 4:
        return 0.2 + 0.2*(dhw-1)
    elif dhw <= 8:
        return 0.8 + (dhw-4)*0.05
    else:
        return 1

## inverse bleached
def health(bleached):
     return int((1.0-bleached)*255.0) 

## These are some patterns
def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        r = g = b = 0
    elif pos < 85:
        r = int(pos * 3)
        g = int(255 - pos * 3)
        b = 0
    elif pos < 170:
        pos -= 85
        r = int(255 - pos * 3)
        g = 0
        b = int(pos * 3)
    else:
        pos -= 170
        r = 0
        g = int(pos * 3)
        b = int(255 - pos * 3)
    return (r, g, b) if ORDER in (neopixel.RGB, neopixel.GRB) else (r, g, b, 0)


def rainbow_cycle(wait):
    for j in range(255):
        for i in range(num_pixels):
            pixel_index = (i * 256 // num_pixels) + j
            pixels[i] = wheel(pixel_index & 255)
        pixels.show()
        time.sleep(wait)

index = 0
def tick(time1=''):
    global index
    time2 = dates[index][:10]
    index = (index + 1)%len(dates)
    #time2 = time.strftime('%I:%M:%S')
    if time2 != time1:
        time1 = time2
        clock_frame.config(text=time2)
    clock_frame.after(200, tick)

root = tk.Tk()
root.title('Digital Clock')
clock_frame = tk.Label(root, font=('times', 100, 'bold'), bg='black', fg='green')
clock_frame.pack(fill='both', expand=1)
root.geometry('700x500')
tick()
root.mainloop()

#while True:
    # Comment this line out if you have RGBW/GRBW NeoPixels
    #pixels.fill((255, 0, 0))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((255, 0, 0, 0))
    #pixels.show()
    #time.sleep(1)

    # Comment this line out if you have RGBW/GRBW NeoPixels
    #pixels.fill((0, 255, 0))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((0, 255, 0, 0))
    #pixels.show()
    #time.sleep(1)

    # Comment this line out if you have RGBW/GRBW NeoPixels
    #pixels.fill((0, 0, 255))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((0, 0, 255, 0))
    #pixels.show()
    #time.sleep(1)

    #rainbow_cycle(0.001)  # rainbow cycle with 1ms delay per step