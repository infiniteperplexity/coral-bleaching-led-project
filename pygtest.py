import pygame
from pygame.locals import *
import csv
import board
import neopixel

pixel_pin = board.D18
num_pixels = 109
ORDER = neopixel.RGB
pixels = neopixel.NeoPixel(
    pixel_pin, num_pixels, brightness=0.2, auto_write=True, pixel_order=ORDER
)


#path = 'C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/'
path = '/home/pi/Documents/GitHub/neopixelpi/'

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
red = (255, 0, 0)
X = 400
Y = 400
display_surface = pygame.display.set_mode((X,Y)) 
pygame.display.set_caption('Hello World!')
font = pygame.font.Font('freesansbold.ttf', 32) 
index = 0
text = font.render(dates[index][:10], True, green, black)   
textRect = text.get_rect()
textRect.center = (X//2, Y//2) 
thermx = 325
thermy = 200
thermw = 10 
fullh = 100
thermh = 100
thermin = 20
thermr = 10


# bleaching function
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

while True:
    text = font.render(dates[index][:10], True, green, black)   
    index = (index + 1) % len(dates)
    display_surface.blit(text, textRect)
    dhw = data[index]
    thermh = thermin+((fullh-thermin)*dhw/10.0) 
    thermc = (health(bleach(dhw)), 0, 0)
    blank = Rect(thermx-thermw/2, fullh, thermw, fullh)
    rect = Rect(thermx-thermw/2, thermy-thermh, thermw, thermh)
    pygame.draw.rect(display_surface, black, blank)
    pygame.draw.rect(display_surface, thermc, rect)
    pygame.draw.circle(display_surface, thermc, (thermx, thermy), thermr, 0)
    print(dates[index])
    pixels.fill((100, 50, 150))
    #pixels.show()
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
    
    pygame.display.update()
    clock.tick(5)
