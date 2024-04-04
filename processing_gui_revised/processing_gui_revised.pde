import processing.serial.*;
Serial arduino;
//String portName = "/dev/ttyACM0";
String portName;
//int BAUD = 9600;
int BAUD = 115200;
//int BAUD = 256000;
// We need to cut down on some of these "magic numbers"
String[] months = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
int[] years = {0, 282, 647, 1012, 1378, 1743, 2108, 2473, 2839, 3204, 3569, 3934, 4300, 4665, 5030, 5395, 5761, 6126, 6491,
                6856, 7222, 7587, 7952, 8317, 8683, 9048, 9413, 9778, 10144, 10509, 10874, 11239, 11605, 11970, 12335, 12700}; // I suspect these are the observation numbers where the years roll over

// might vary per screen...maroon laptop is looking okay at 0.75, 200, 0.
float SCALE = 0.75;
int TRANSLATE_X = 200;
int TRANSLATE_Y = 0;

int world_x = -125;
int world_y = 550;
int world_width = 2500;
int world_height = 1000;

PFont font;
PFont lcdTiny;
PFont lcdFont;
PFont lcdSmall;
int index = 0;
//int wait = 50;
int wait = 100;
long last = 0;
PShape world;
PShape button;
ArrayList<Reef> reefs = new ArrayList<Reef>();
int tick;
float fade = 10;
int selected = 0;
boolean bookmarks = true;
//https://pae-paha.pacioos.hawaii.edu/erddap/griddap/dhw_5km.html
//https://en.wikipedia.org/wiki/List_of_reefs
//https://pae-paha.pacioos.hawaii.edu/erddap/griddap/dhw_5km.csv?CRW_DHW[(2020-01-01T12:00:00Z):1:(2020-05-17T12:00:00Z)][(17.175):1:(17.175)][(-87.325):1:(-87.325)],CRW_SST[(2020-01-01T12:00:00Z):1:(2020-05-17T12:00:00Z)][(17.175):1:(17.175)][(-87.325):1:(-87.325)]

void setup(){
  // I'm pretty sure these magic numbers are related to longitude and latitude
  reefs.add(new Reef(2055, 1160, "Great Barrier Reef", "great"));
  //reefs.add(new Reef(1289, 852, "New Caledonia Barrier Reef", "newcaledonia"));
  reefs.add(new Reef(1885, 1000, "Tubbataha Reef", "tubbataha"));
  reefs.add(new Reef(1320, 920, "Red Sea Reef System", "redsea"));
  //reefs.add(new Reef(515, 900, "Florida Keys Reef Tract", "florida"));
  reefs.add(new Reef(515, 895, "Florida Keys Reef Tract", "florida"));
  //reefs.add(new Reef(467, 942, "Belize Barrier Reef", "belize"));
  reefs.add(new Reef(455, 955, "Belize Barrier Reef", "belize"));
  world = loadShape("Continents.svg");
  //button = loadShape("button.svg");
  button = loadShape("button_fixed.svg"); // fixed a broken SVG file
  fullScreen();
  font = loadFont("ArialMT-48.vlw");
  //DS-Digital by Dusit Supasawat
  lcdTiny = createFont("DS-DIGI.TTF", 36);
  lcdSmall = createFont("DS-DIGI.TTF", 72);
  lcdFont = createFont("DS-DIGI.TTF", 144);
  tick = 0;
  try
  {
    portName = Serial.list()[0];
    print(portName);
    arduino = new Serial(this, portName, BAUD);
  }
  catch(Exception e)
  {
    System.err.println("Error initializing Serial port!\nPlease check connections and port settings");
    e.printStackTrace();
  }
}
void draw(){
  scale(SCALE, SCALE);
  translate(TRANSLATE_X, TRANSLATE_Y);
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
    int year = Integer.valueOf(date.substring(0,4));
    String month = months[Integer.valueOf(date.substring(5,7))-1];
    background(0, 0, 0);
    stroke(0,0,0);
    strokeWeight(1);
    //// World Map
    world.disableStyle();
    fill(0,255,00);
    shape(world,world_x,world_y, world_width, world_height); // these must be the size I guess?
    fill(0, 255, 0);
    
    //// Text
    textFont(lcdFont);
    // Name
    text(name, 150, 150+50);
    // Date
    text(" -  +  " + year + " " + month, 150, 300+50);
    // Stress (DHW)
    text("stress: " + nf(dhw,1,2), 150, 450+50);
    // Celsius
    text(nf(temp,2,2), 940, 450+50);
    text(" C", 1265, 450+50);
    circle(1285, 370+50, 25);
    fill(0,0,0);
    circle(1285, 370+50, 12);
    // Farenheit
    fill(0, 255, 0);    
    circle(1795, 370+50, 25);
    fill(0,0,0);
    circle(1795, 370+50, 12);
    fill(0, 255, 0);
    text(nf(1.8*temp+32,2,2), 1450, 450+50);
    text(" F", 1780, 450+50);
    
    tick += 1;
    for (int i = 0; i < reefs.size(); i++)
    {
      Reef r = reefs.get(i);
      if (selected == i)
      {
        fill(0,255,0);
      }
      else
      {
        fill(0,255,0);
      }
      int radius = 30;
      strokeWeight(3);
      if (selected == i)
      {
        radius = 50;
        strokeWeight(5);
      }
      circle(r.X, r.Y, 2*radius);
      pushMatrix();
      translate(r.X,r.Y);
      rotate(4*radians(tick));
      world.disableStyle();
      fill(0,0,0);
      shape(button,-radius,-radius,2*radius,2*radius);
      popMatrix();
      noFill();

    }
    stroke(0, 255, 0);
    strokeWeight(5);
    int dashes = 24;
    float unit = PI/dashes;
    for (int i = 0; i < dashes; i++)
    {     
      arc(214, 305, 80+2*sin(tick), 80+2*sin(tick), 2*i*unit, (2*i+0.5)*unit );
      arc(342, 305, 80+2*sin(tick), 80+2*sin(tick), 2*i*unit, (2*i+0.5)*unit );
    }
    
    last = millis();
    try
    {
      arduino.write(Math.round(255*(1.0-bleach(dhw))));
    }
    catch(Exception e)
    {
      ;
    }
    index = (index + bump) % t.getRowCount();
    //last = millis();
    
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

int getYear()
{
  for (int i = years.length-1; i>=0; i--)
  {
    if (index >= years[i])
    {
      return i;
    }
  }
  return 0;
}
void keyPressed()
{
  exit();
}

void mousePressed() {
  //int effective_x = int(SCALE*(mouseX - TRANSLATE_X));
  //int effective_x = int((mouseX - TRANSLATE_X)/SCALE);
  int effective_x = int(mouseX/SCALE) - TRANSLATE_X;
  int effective_y = int(mouseY/SCALE) - TRANSLATE_Y;
  
  for (int i = 0; i < reefs.size(); i++)
  {
    Reef r = reefs.get(i);
    if (Math.sqrt((effective_x - r.X)*(effective_x- r.X) + (effective_y - r.Y)*(effective_y- r.Y))<30)
    {
      selected = i;
    }
  }
  if (effective_x>= 214-45 && effective_x <= 214+45 && effective_y >= 305-45 && effective_y <= 305+45)
  {
    index = years[Math.max(0,getYear()-1)];
  }
  else if (effective_x >= 342-45 && effective_x <= 342+45 && effective_y >= 305-45 && effective_y <=305+45)
  {
    index = years[Math.min(getYear()+1, years.length-1)];
  }
}
