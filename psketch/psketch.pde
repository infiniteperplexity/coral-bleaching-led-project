import processing.serial.*;
Serial arduino;
String portName = "/dev/ttyACM0";
String path = "C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/";
//string path = "/home/pi/Documents/GitHub/neopixelpi/";
Table table;
PFont font;
int index = 0;
int wait = 20;
long last = 0;

void setup(){
  size(750, 750);
  table = loadTable("concat.csv", "header");
  font = loadFont("ArialMT-48.vlw");
  textFont(font);
  try
  {
    arduino = new Serial(this, portName, 9600);
  }
  catch(Exception e)
  {
    System.err.println("Error initializing Serial port!\nPlease check connections and port settings");
    e.printStackTrace();
  }
}
void draw(){
  long now = millis();
  if (now - last >= wait)
  {
    TableRow tr = table.getRow(index);
    int dhw = parseInt(tr.getString(3));
    String date = tr.getString(0).substring(0,10);
    background(0, 0, 0);
    fill(0, 255, 0);
    //text(date, 250, 200);
    text(now - last, 250, 250);
    fill(255*(1.0-bleach(dhw)), 0, 0);
    circle(375, 375, 150);
    try
    {
      arduino.write(Math.round(255*(1.0-bleach(dhw))));
    }
    catch(Exception e)
    {
      ;
    }
    last = millis();
    index += 1;
  }
  //delay(wait);
}

float bleach(float dhw)
{
  if (dhw <= 1)
  {
    return 0.2*dhw;
  }
  else if (dhw <= 4)
  {
    return 0.2 + 0.2*(dhw-1);
  }
  else if (dhw <= 8)
  {
    return 0.8 + (dhw-4)*0.05;
  }
  return 1;
}
