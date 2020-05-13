import pygame
from pygame.locals import *
import csv
#import board
#import neopixel

#pixel_pin = board.D18
num_pixels = 109
#ORDER = neopixel.GRB
#pixels = neopixel.NeoPixel(
#    pixel_pin, num_pixels, brightness=0.2, auto_write=False, pixel_order=ORDER
#)

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
pygame.init()
clock = pygame.time.Clock()
white = (255, 255, 255) 
green = (0, 255, 0) 
blue = (0, 0, 128)
black = (0, 0, 0)
X = 400
Y = 400
display_surface = pygame.display.set_mode((X,Y)) 
pygame.display.set_caption('Hello World!')
font = pygame.font.Font('freesansbold.ttf', 32) 
index = 0
text = font.render(dates[index][:10], True, green, black)	
textRect = text.get_rect()
textRect.center = (X//2, Y//2) 
while True:
	#display_surface.fill(white) 
	text = font.render(dates[index][:10], True, green, black)	
	index = (index + 1) % len(dates)
	display_surface.blit(text, textRect) 
	for event in pygame.event.get():
		if event.type == QUIT:
			pygame.quit()
			sys.exit()
	
	pygame.display.update()
	clock.tick(50)


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
