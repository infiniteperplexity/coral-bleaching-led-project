#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NPIXELS 18*18*4

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(109, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPIXELS, PIN, NEO_GRBW  + NEO_KHZ800);


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

char serialChar = 255;


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

// enumerate patterns, which is the top-level choice
enum Pattern {RAINBOW, TEST};


int timer = 0;
int counter;
float health = 1.0;
void loop() {
  // we probably need a better way to choose patterns but this will do for now
  static Pattern activePattern = TEST;
  // probably should do this instead
  //static void (*pattern)(void) = &rainbow;
  getSerial(); 
  if (activePattern == RAINBOW)
  {
    rainbow(20);
    //rainbow(100);
  }
  else if (activePattern == TEST)
  {
    test(20);
  }
  else
  {
    rainbow(20);
  }
}
// get health data from the serial connection
void getSerial()
{
  if (Serial.available() > 0) {
    serialChar = Serial.read();
    health = byte(serialChar)/255.0;
    //Serial
  }
}

// change the saturation based on the health of the coral
void bleachedDispatch(uint16_t pixel, uint32_t color)
{
    float bleached = 1.0 - health;
    byte r = (color >> 16);
    byte g = (color >>  8);
    byte b = (uint8_t) color;
    byte r2 = r + (255 - r) * bleached;
    byte g2 = g + (255 - g) * bleached;
    byte b2 = b + (255 - b) * bleached;
    strip.setPixelColor(pixel, strip.Color(r2, g2, b2));
}

void rainbow(uint8_t wait) {
  uint16_t j = counter;
  if (millis() - timer > wait)
  {
    timer = millis();
    for (uint16_t i=0; i<strip.numPixels(); i++) {
      bleachedDispatch(i, Wheel((i+j) & 255));
    }
    counter = (counter+1)%256;
    // perhaps this should happen a little less often
    strip.show();
  }
}

void test(uint8_t wait) {
  uint16_t j = counter;
  if (millis() - timer > wait)
  {
    timer = millis();
    if (j < 64)
    {
      for (uint16_t i=0; i<NPIXELS; i++) {
        bleachedDispatch(i, strip.Color(255, 0, 0));
      }
    }
    else if (j < 128)
    {
      for (uint16_t i=0; i<NPIXELS; i++) {
        bleachedDispatch(i, strip.Color(0, 255, 0));
      }
    }
    else if (j < 128+64)
    {
      for (uint16_t i=0; i<NPIXELS; i++) {
        bleachedDispatch(i, strip.Color(0, 0, 255));
      }
    }
    else
    {
      for (uint16_t i=0; i<NPIXELS; i++) {
        strip.setPixelColor(i, strip.Color(0,0,0,255));
      }
    }
    counter = (counter+1)%256;
    // perhaps this should happen a little less often
    strip.show();
  }
}





// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
