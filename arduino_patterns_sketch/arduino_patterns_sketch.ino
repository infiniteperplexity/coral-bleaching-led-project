// As of 8/10/2022, I archived all other versions of this code; the multiple versions were getting too confusing.

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
  #include <math.h>
#endif

#define PIN 6
#define NLEDS 10
//3680
#define TRIANGLE 136
#define SQUARE 324
#define SIDE 18

//#ifdef __arm__
//// should use uinstd.h to define sbrk but Due causes a conflict
//extern "C" char* sbrk(int incr);
//#else  // __ARM__
//extern char *__brkval;
//#endif  // __arm__
 
//int freeMemory() {
//  char top;
//#ifdef __arm__
//  return &top - reinterpret_cast<char*>(sbrk(0));
//#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
//  return &top - __brkval;
//#else  // __arm__
//  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
//#endif  // __arm__
//}

// ************ Define the zig-zag wire paths for square and triangular panels
int square[18][18] = {
  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18},
  {36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19},
  {37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54},
  {72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55},
  {73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90},
  {108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91},
  {109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126},
  {144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127},
  {145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162},
  {180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163},
  {181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198},
  {199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216},
  {234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217},
  {235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252},
  {270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 259, 258, 257, 256, 255, 254, 253},
  {271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288},
  {306, 305, 304, 303, 302, 301, 300, 299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289},
  {307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324}
};
int triangle[18][18] = {
  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0, 0},
  {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 0, 0, 0},
  {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 0, 0, 0, 0},
  {58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 0, 0, 0, 0, 0},
  {59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 0, 0, 0, 0, 0, 0},
  {81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 0, 0, 0, 0, 0, 0, 0},
  {82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 0, 0, 0, 0, 0, 0, 0, 0},
  {100, 99, 98, 97, 96, 95, 94, 93, 92, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {101, 102, 103, 104, 105, 106, 107, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {115, 114, 113, 112, 111, 110, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {116, 117, 118, 119, 120, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {126, 125, 124, 123, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {127, 128, 129, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {133, 132, 131, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {134, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Initialize mapping arrays for all the panels
int square3[18][18] = {{0}};
int triangle4[18][18] = {{0}};
int triangle1[18][18] = {{0}};
int square2[18][18] = {{0}};
int square5[18][18] = {{0}};
int triangle6[18][18] = {{0}};
int triangle7[18][18] = {{0}};
int square8[18][18] = {{0}};
int square9[18][18] = {{0}};
int triangle10[18][18] = {{0}};
int triangle11[18][18] = {{0}};
int square12[18][18] = {{0}};
int triangle13[18][18] = {{0}};
int square14[18][18] = {{0}};
int square15[18][18] = {{0}};
int triangle16[18][18] = {{0}};


//******************* These are define the masks for pixels that are covered, but currently there are lots of mistakes*************
int mask3[] = {1,5,7,8,10,15,16,17,18,19,23,25,26,31,34,37,38,50,52,54,56,60,62,63,64,65,66,67,68,74,75,77,86,87,90,91,93,97,100,101,105,109,110,114,115,120,122,125,
 127,131,138,140,142,149,152,154,155,157,159,162,155,168,171,176,178,179,180,184,196,197,198,199,200,201,204,205,207,208,209,210,212,216,218,219,221,227,232,233,234,
 235,236,237,239,240,241,245,246,248,252,254,255,257,261,266,267,268,269,270,271,272,273,279,290,293,294,295,296,297,299,300,301,302,303,304,305,306,307,308,309,314,
 320,321,323};

//16,31,45,58,70,81,91,100,108,115,121,126,130,133,135,136
int mask4[] = {1,3,6,10,12,16,18,19,20,22,23,24,25,30,32,35,36,37,41,43,44,45,46,49,50,52,55,58,60,65,67,71,73,76,80,84,85,86,93,98,100,101,106,108,
110,115,116,118,121,123,124,125,126,127,128,129,131,132,133,134,135,136};

int mask7[] = {1,2,3,5,6,7,8,9,13,14,15,16,17,18,20,21,22,24,26,27,28,29,30,31,32,37,39,40,41,42,43,44,45,46,47,38,39,50,51,57,58,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,76,84,85,86,87,88,89,90,91,92,93,94,96,103,105,107,109,112,113,116,117,122,125,127,131,133,135};

int mask8[] = {4,6,8,11,13,15,16,18,19,21,23,26,29,32,34,35,41,47,50,54,55,61,66,73,28,80,81,82,83,90,94,97,102,106,110,113,115,116,118,120,122,127,130,
131,132,136,140,142,146,150,151,153,155,161,162,163,164,165,175,178,183,186,188,190,193,194,195,197,198,199,200,202,212,214,219,221,224,225,226,229,231,
234,235,246,239,245,252,253,268,269,270,271,272,273,275,277,281,282,284,286,293,302,303,304,305,306,307,308,309,310,311,312,314,316,319,323};

// ************ Define the layout of the panels and the data path ********************************
// this is going to contain the actual panels themselves, using n-1 numbering (e.g. panel #1 is index 0)
int panels[16][18][18] = {{{0}}};
int npixels[16] = {TRIANGLE, SQUARE, SQUARE, TRIANGLE, SQUARE, TRIANGLE, TRIANGLE, SQUARE, SQUARE, TRIANGLE, TRIANGLE, SQUARE, TRIANGLE, SQUARE, SQUARE, TRIANGLE};
// this layout assumes normal computer x/y coordinates, where y=0 is at the top
// the cell values refer to indices in the "panels" array
int layout[4][4] = {
  {0, 4, 8, 12},
  {1, 5, 9, 13},
  {2, 6, 10, 14},
  {3, 7, 11, 15}
};
// the cell values refer to indices in the "panels" array
int path[16] = {2, 3, 7, 6, 10, 11, 15, 14, 13, 12, 8, 9, 5, 4, 0, 1}; // the path begins at panel #3 (n+1), heads to #4, and so on, all ending in panel #2
// in theory this is supposed to map from the (n-1) panel number to the path position, but I don't know if this has ever been used or tested
int invpath[16] = {14, 15, 0, 1, 13, 12, 3, 2, 10, 11, 4, 5, 9, 8, 7, 6};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLEDS, PIN, NEO_GRBW  + NEO_KHZ800);

// if no serial input is found, simply assume you get a constant stream of 255s
char serialChar = 255;

//this is just for blinking the test LED on the board itself
unsigned long previousMillis = 0;  
int ledState = LOW;  

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  for (int i=0; i<SIDE; i++)
  {
    // **********This code rotates the software representation of each panel into the correct orientation**********
    // !!!the orientations have been validated but it's possible a new error was introduced while doing so
    for (int j=0; j<SIDE; j++)
    {
      square3[i][j] = square[i][j];
      triangle4[i][j] = triangle[i][17-j];
      triangle1[i][j] = triangle[17-i][17-j];
      square2[i][j] = square[17-i][j];
      square5[i][j] = square[j][17-i];
      triangle6[i][j] = triangle[i][j];
      //if (triangle[i][j]>0)
      // the strip on triangle 7 runs from the opposite corner so it has to be handled differently
      // was the above where things went haywire?  I think this is the old code
       //     if (triangle4[17-i][17-j]>0)
      //{
        // the strip on triangle 7 runs from the opposite corner so it has to be handled differently
        //triangle7[i][j] = TRIANGLE + 1 - triangle4[17-i][17-j];
      //}
      // first debug test...take triangle7 off the table
      if (triangle[17-i][j]>0) {
        triangle7[i][j] = TRIANGLE + 1 - triangle[17-i][j];
      }
      square8[i][j] = square[17-j][17-i];
      square9[i][j] = square[j][i];
      triangle10[i][j] = triangle[i][17-j];
      triangle11[i][j] = triangle[17-i][17-j]; 
      square12[i][j] = square[17-j][i];
      triangle13[i][j] = triangle[17-i][j];
      square14[i][j] = square[17-i][j];
      square15[i][j] = square[i][17-j];
      triangle16[i][j] = triangle[i][j];
    }
  }
//*************this is the code that implements masking ***********************
  bool useMasks = false;
  if (useMasks == true)
  {
    for (int k=0; k<(sizeof(mask3)/sizeof(int)); k++)
    {
      for (int i=0; i<SIDE; i++)
      {
        for (int j=0; j<SIDE; j++)
        {
           //numbering goes from 1, not 0;
          if (square3[i][j]==mask3[k])
          {
            square3[i][j] = -square3[i][j]; 
          }
        }
      }
    }
    for (int i=0; i<SIDE; i++)
    {
      for (int j=0; j<SIDE; j++)
      {
        for (int k=0; k<(sizeof(mask4)/sizeof(int)); k++)
        {
          if (triangle4[i][j]==mask4[k])
          {
            triangle4[i][j] = -triangle4[i][j]; 
          }
        }
      }
    }
    for (int i=0; i<SIDE; i++)
    {
      for (int j=0; j<SIDE; j++)
      {
        for (int k=0; k<(sizeof(mask7)/sizeof(int)); k++)
        {
          if (triangle7[i][j]==mask7[k])
          {
            triangle7[i][j] = -triangle7[i][j]; 
          }
        }
      }
    }
    for (int i=0; i<SIDE; i++)
    {
      for (int j=0; j<SIDE; j++)
      {
        for (int k=0; k<(sizeof(mask8)/sizeof(int)); k++)
        {
          if (square8[i][j]==mask8[k])
          {
            square8[i][j] = -square8[i][j]; 
          }
        }
      }
    }
  }
  // ***** This copies by value to the panels array...note that it's not by value, not reference, and any further changes will have no effect *****
  memcpy(panels[0], triangle1, sizeof(panels[0]));
  memcpy(panels[1], square2, sizeof(panels[0]));
  memcpy(panels[2], square3, sizeof(panels[0]));
  memcpy(panels[3], triangle4, sizeof(panels[0]));
  memcpy(panels[4], square5, sizeof(panels[0]));
  memcpy(panels[5], triangle6, sizeof(panels[0]));
  memcpy(panels[6], triangle7, sizeof(panels[0]));
  memcpy(panels[7], square8, sizeof(panels[0]));
  memcpy(panels[8], square9, sizeof(panels[0]));
  memcpy(panels[9], triangle10, sizeof(panels[0]));
  memcpy(panels[10], triangle11, sizeof(panels[0]));
  memcpy(panels[11], square12, sizeof(panels[0]));
  memcpy(panels[12], triangle13, sizeof(panels[0]));
  memcpy(panels[13], square14, sizeof(panels[0]));
  memcpy(panels[14], square15, sizeof(panels[0]));
  memcpy(panels[15], triangle16, sizeof(panels[0]));

  //**********this code maps the values in the panels array to pixel numbers, so when you look at a position within the array you can see which pixel in the chain it corresponds to (validated)
  int maxpixel = 0;
  int tally =0;
  // for each panel...
  for (int k=0; k<16; k++)
  {
    // start counting with the highest pixel number seen so far, so number the pixels sequentially from panel to panel
    tally = maxpixel;
    for (int i=0; i<SIDE; i++)
    {
      for (int j=0; j<SIDE; j++)
      {
        // this looks at the panel number that's kth in the path, then takes the sequential pixel number within that panel
        int pixel = panels[path[k]][i][j];
        
        // if the pixel is masked, prepare to check it against the highest-pixel-seen but map a blank pixel
        if (pixel < 0)
        {
          pixel = tally - pixel;
          panels[path[k]][i][j] = 0;
        }
        // if the pixel is displayed, prepare to check it against highest pixel seen and map the panel to its place in the path
        else if (pixel > 0)
        {
          pixel += tally;
          panels[path[k]][i][j] = pixel;
        }
        // otherwise it's off the corner of a triangle and shouldn't count as part of the path
        // keep track of highest pixel seen
        maxpixel = max(pixel, maxpixel);
      }
    }
  }

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

// enumerate patterns, which is the top-level choice
enum Pattern {RAINBOW, TEST};



int timer = 0;
float health = 1.0;
int counter = 0;
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    //digitalWrite(LED_BUILTIN, ledState);
  }
  getSerial();
  // at this stage, we probably want an option to choose which pattern we're showing...right now there's just the main rainbow, but it would be nice to have some debugging patterns as well
  static Pattern activePattern = RAINBOW;
  if (activePattern == RAINBOW)
  {
    traverse_grid();
  }
  else if (activePattern == TEST)
  {
    
  }
  strip.show();
  // I'm guessing 6 is a magic number that makes the speed work well?
  counter += 6;
}
// get health data from the serial connection
void getSerial()
{
  if (Serial.available() > 0) {
    serialChar = Serial.read();
    health = byte(serialChar)/255.0;
    
    //Serial
  }
  //health = 0.0;
}

void traverse_grid()
{
  for (int i=0; i<(4*SIDE); i++)
  {
    for (int j=0; j<(4*SIDE); j++)
    {
      int pixel = xy2pixel(i,j);
      if (pixel >= 0)
      {
        // this is where the actual pattern is defined...maybe should structure this differently
        int angle = xy2angle(i,j);
        int color = rainbow(angle);
        //strip.setPixelColor(pixel, color);
        bleachedDispatch(pixel, color);
        //strip.setPixelColor(pixel, strip.Color(0,0,0));
      }
    }
  }
}
// *** map an x,y coordinate to a pixel
int xy2pixel(int x, int y)
{
  int x0 = x/SIDE;
  int y0 = y/SIDE;
  int x1 = x - x0*SIDE;
  int y1 = y - y0*SIDE;
  return panels[layout[x0][y0]][x1][y1]-1;
}
// *** map an x,y coordinate to an angle (relative to the center)
double xy2angle(int y, int x)
{
  return (atan2(y-2*SIDE, x-2*SIDE)*180)/M_PI;
}


int rainbow(int angle)
{
  // let's not worry about animation just yet
  // start with exactly one rainbow for now
  // the wheel() function takes a value from 0 to 255
  int pos = map((angle + counter)%360, 0, 360, 0, 255);
  int color = wheel(pos);
  // I'm not sure if this is actually how frequently we want to loop
  //counter = (counter + 1) % 360;
  return color;
}

// change the saturation based on the health of the coral
void bleachedDispatch(uint16_t pixel, uint32_t color)
{
    float bleached = 1.0 - health;
    byte r = (color >> 16);
    byte g = (color >>  8);
    byte b = (uint8_t) color;
    //byte r2 = r + (255 - r) * bleached;
    //byte g2 = g + (255 - g) * bleached;
    //byte b2 = b + (255 - b) * bleached;
    //strip.setPixelColor(pixel, strip.Color(r2, g2, b2));
    // this should save power and it might even look better
    strip.setPixelColor(pixel, strip.Color(health * r, health * g, health * b, bleached * 255));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
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

// This code was in another version...I think it prints to serial the pixel mapping

//String msg = "";
//   //for this kind of thing, we need to nest x loops within y loops to make the display intuitively match cartesian coordinates
//  for (int j=0; j<(4*SIDE); j++)
//  {
//    msg = String(j) + String(": ");
//    for (int i=0; i<(4*SIDE); i++)
//    {
//      int i0 = i/SIDE;
//      int j0 = j/SIDE;
//      int i1 = i - i0*SIDE;
//      int j1 = j - j0*SIDE;
//      int pixel = panels[layout[i0][j0]][i1][j1] - 1;
//      String spix = (String(pixel)+String("     ")).substring(0,5);
//      
//      msg = msg + spix;
//    }
//    Serial.println(msg);
//    msg = "";


// **** This was an older test that linearly addresses the pixels
//void linear_test(int color) {
//  int c = color;
//  int pixel;
//  bool alternateColors = false;
//  for (int i=0; i<NPIXELS; i++)
//  {
//    int ii = i+1;
//    if (alternateColors == true)
//     {
//      if (ii%10 == 1)
//      {
//        c = strip.Color(0,0,0,255);
//      }
//      else if (ii%10 == 2)
//      {
//        c = strip.Color(255,0,0);
//      }
//      else if (ii%10 == 3)
//      {
//        c = strip.Color(255,255,0);
//      }
//      else if (ii%10 == 4)
//      {
//        c = strip.Color(0,255,0);
//      }
//      else if (ii%10 == 5)
//      {
//        c = strip.Color(0,0,0);
//      }
//      else if (ii%10 == 6)
//      {
//        c = strip.Color(0,255,255);
//      }
//      else if (ii%10 == 7)
//      {
//        c = strip.Color(0,0,255);
//      }
//      else if (ii%10 == 8)
//      {
//        c = strip.Color(255,0,255);
//      }
//      else if (ii%10 == 9)
//      {
//        c = strip.Color(55,0,0);
//      }
//      else if (ii%10 == 0)
//      {
//        c = strip.Color(0,55,0);
//      }
//    }
//    strip.setPixelColor(i, c);
//  }
//  strip.show();
//}
