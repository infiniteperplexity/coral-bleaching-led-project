import RPi.GPIO as GPIO
outpin = 18
GPIO.setmode(GPIO.BOARD)
GPIO.setup(outpin, GPIO.OUT)
while true:
	GPIO.output(outpin, GPIO.HIGH)
	print("HI")
    time.sleep(1)
    GPIO.output(outpin, GPIO.LOW)
    print("LO")
    time.sleep(1)

GPIO.cleanup()