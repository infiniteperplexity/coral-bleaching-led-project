import processing.serial.*;
Serial arduino;
String portName = "/dev/ttyACM0";
String path = "C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/";
//string path = "/home/pi/Documents/GitHub/neopixelpi/";
PFont font;
PFont lcdFont;
int index = 0;
int wait = 50;
long last = 0;
PImage img;
ArrayList<Reef> reefs = new ArrayList<Reef>();
int selected = 0;
//https://pae-paha.pacioos.hawaii.edu/erddap/griddap/dhw_5km.html
//https://en.wikipedia.org/wiki/List_of_reefs
//https://pae-paha.pacioos.hawaii.edu/erddap/griddap/dhw_5km.csv?CRW_DHW[(2020-01-01T12:00:00Z):1:(2020-05-17T12:00:00Z)][(17.175):1:(17.175)][(-87.325):1:(-87.325)],CRW_SST[(2020-01-01T12:00:00Z):1:(2020-05-17T12:00:00Z)][(17.175):1:(17.175)][(-87.325):1:(-87.325)]

void setup(){
  reefs.add(new Reef(1249, 850, "Great Barrier Reef", "great"));
  reefs.add(new Reef(1289, 852, "New Caledonia Barrier Reef", "newcaledonia"));
  reefs.add(new Reef(1109, 705, "Tubbataha Reef", "tubbataha"));
  reefs.add(new Reef(748, 630, "Red Sea Coral Reef", "redsea"));
  reefs.add(new Reef(1823, 616, "Florida Keys Reef", "florida"));
  reefs.add(new Reef(1797, 650, "Belize Barrier Reef", "belize"));
  
  fullScreen();
  
  font = loadFont("ArialMT-48.vlw");
  //DS-Digital by Dusit Supasawat
  lcdFont = createFont("DS-DIGI.TTF", 144);
  img = loadImage("reefmap.jpg");
  
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
    Table t = reefs.get(selected).Data;
    // this could happen if the number of rows is off by one or two when you switch reefs, due to downloading on different dates
    if (index > t.getRowCount())
    {
      index = 0;
    }
    TableRow tr = t.getRow(index);
    String name = reefs.get(selected).Name;
    float dhw = tr.getFloat(3);
    float temp = tr.getFloat(4);
    String date = tr.getString(0).substring(0,7);
    background(0, 0, 0);
    image(img, 500, 150, width - 500, height - 300);
    fill(0, 255, 0);
    textFont(font);
    text(name, 35, 250);
    textFont(lcdFont);
    text(date, 35, 450);
    text(dhw, 85, 600);
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
    index = (index + bump) % t.getRowCount();
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
  String Name;
  Table Data;
  Reef(int x, int y, String name, String fname)
  {
    X = x;
    Y = y;
    Name = name;
    Data = loadTable(fname +".csv", "header");
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
      selected = i;
    }
  }
}
