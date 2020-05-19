import processing.serial.*;
Serial arduino;
String portName = "/dev/ttyACM0";
String path = "C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/";
//string path = "/home/pi/Documents/GitHub/neopixelpi/";
Table table;
PFont lcdFont;
int index = 0;
int wait = 50;
long last = 0;
PImage img;
ArrayList<Reef> reefs = new ArrayList<Reef>();

//https://en.wikipedia.org/wiki/List_of_reefs

void setup(){
  reefs.add(new Reef(1249, 850));
  fullScreen();
  table = loadTable("concat.csv", "header");
  //font = loadFont("ArialMT-48.vlw");
  //DS-Digital by Dusit Supasawat
  lcdFont = createFont("DS-DIGI.TTF", 144);
  img = loadImage("reefmap.jpg");
  textFont(lcdFont);
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
    // this is how many "steps" the time advances
    int bump = int(now - last) / wait;
    TableRow tr = table.getRow(index);
    float dhw = tr.getFloat(3);
    String date = tr.getString(0).substring(0,7);
    background(0, 0, 0);
    image(img, 500, 150, width - 500, height - 300);
    fill(0, 255, 0);
    text(date, 35, 250);
    text(dhw, 85, 400);
    int health = parseInt(255*(1.0-bleach(dhw)));
    fill(0, health, 0);
    for (int i = 0; i < reefs.size(); i++)
    {
      Reef r = reefs.get(i);
      circle(r.X, r.Y, 35);
    }
    try
    {
      arduino.write(Math.round(255*(1.0-bleach(dhw))));
    }
    catch(Exception e)
    {
      ;
    }
    index = (index + bump) % table.getRowCount();
    last = millis();
    
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

class Reef
{
  int X;
  int Y;
  Reef(int x, int y)
  {
    X = x;
    Y = y;
  }
}

void keyPressed()
{
  exit();
}

void mousePressed() {
  for (int i = 0; i < reefs.size(); i++)
  {
    Reef r = reefs.get(i);
    if (Math.sqrt((mouseX - r.X)*(mouseX - r.X) + (mouseY - r.Y)*(mouseY - r.Y))<17.5)
    {
      println(i);
    }
  }
}
