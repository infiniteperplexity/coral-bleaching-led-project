import RPi.GPIO as GPIO
import time
outpin = 12
GPIO.setmode(GPIO.BOARD)
GPIO.setup(outpin, GPIO.OUT)
while True:
    GPIO.output(outpin, GPIO.HIGH)
    print("HI")
    time.sleep(1)
    GPIO.output(outpin, GPIO.LOW)
    print("LO")
    time.sleep(1)

GPIO.cleanup()