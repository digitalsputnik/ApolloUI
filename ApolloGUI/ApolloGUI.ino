/*
    Simple Touch Drawing sample for WT32-SC01
*/
#define LGFX_AUTODETECT     // Autodetect board
#define LGFX_USE_V1         // set to use new version of library

#include <LovyanGFX.hpp>    // main library
#include <FastLED.h>
#include <Wire.h>

#include <EEPROM.h>

#include <WiFi.h>
#include "AsyncUDP.h"

//led control
#define NUM_LEDS 6
#define LED_DATA_PIN 4

static LGFX lcd;            // declare display variable

hw_timer_t * timer = NULL;


// Define the array of leds
CRGB leds[NUM_LEDS];
AsyncUDP udp;


int count=0;
bool state=0;
// Variables for touch x,y
static int32_t x,y;
int X,Y;

volatile bool tick_update=false;
int intensity_val, prev_intensity_val;

//Adafruit_Image img;

String startupString[] = {  
                            // Group Box
                            "B000 000w320h054r046g046b046",
                            "B010 010w300h034r079g079b079",
                              //text
                            "T160 016a1f4r255g255b255r079g079b079Apollo0189",
                            "T030 016a0f4r255g255b255r079g079b079<",
                            "T290 016a2f4r255g255b255r079g079b079>",

                            //int
                            "B010 064w300h040r046g046b046",
                            "T030 074a0f4r255g213b046r046g046b046Intensity",
                            "T290 074a2f4r079g079b079r046g046b046100%",
                            
                            //temp
                            "B010 114w300h040r046g046b046",
                            "T030 124a0f4r255g213b046r046g046b046Temperature",
                            "T290 124a2f4r079g079b079r046g046b0465600K",

                            //sat
                            "B010 164w300h040r046g046b046",
                            "T030 174a0f4r255g213b046r046g046b046Saturation",
                            "T290 174a2f4r079g079b079r046g046b0460%",
                            
                            //hue
                            "B010 214w300h040r046g046b046",
                            "T030 224a0f4r255g213b046r046g046b046Hue",
                            "T290 224a2f4r079g079b079r046g046b0460%",   

                            //FX
                            "B010 264w300h040r046g046b046",
                            "T030 274a0f4r255g213b046r046g046b046Fx",
                            "T290 274a2f4r079g079b079r046g046b040%",

                            //Manage Viewers
                            "B010 314w300h040r046g046b046",
                            "T030 324a0f4r255g213b046r046g046b046Manage Viewers"
                            };


String mainProgramString[255];

void tick10ms() {
  tick_update=true;
  }

void setup(void)
{
    
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &tick10ms, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  
  Serial.begin(115200);
  Wire.begin(25,5);
  lcd.init();
  lcd.setColorDepth(24);

  if (!EEPROM.begin(255)) {
    Serial.println("Failed to initialise EEPROM");
  }
  
  Serial.print("Generating main program stack");
  for(int i = 0;i<255;i++) {
    mainProgramString[i] = String("");
  }
  Serial.print("program stack generated");
  


  // Setting display to landscape
  //if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 1);

  FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical

  leds[0] = CRGB::Cyan;
  leds[1] = CRGB::Cyan;
  leds[2] = CRGB::Cyan;
  leds[3] = CRGB::Cyan;
  leds[4] = CRGB::Green;
  leds[5] = CRGB::Magenta;
  FastLED.show();

  //i2cScan();

  getEncoder();
  prev_intensity_val = intensity_val;

  initWiFi();

}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("DS","SputnikulOn4Antenni");
  Serial.print("Connecting to WiFi ..");
  parseDrawInput("B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000Connecting to WiFi ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  parseDrawInput("B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000"+WiFi.localIP());
}

void i2cScan() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }


}

void getEncoder() {
  //connect to encoder
  uint8_t data[2];
  Wire.beginTransmission(0x40);
  Wire.write(0x10);
  Wire.endTransmission();

  //read encoder value
  Wire.requestFrom(0x40, 2);
  for (int i = 0; i<2; i++) {
    data[i] = Wire.read();
  }

  intensity_val = (signed short int)((data[0]) | (data[1]) << 8);

  if(prev_intensity_val!=intensity_val) {
    //propIntensity._update=true;
    //prev_intensity_val=intensity_val;
  }
}

void parseDrawInput(String _inputString2) {
// Some simple strings to operate
// Clear Screen: B000 000w320h480r000g000b000
// dark cyan horizontal stripe: B000 239w320h002r000g030b100
// text test: T160 016a1f4r255g255b255r079g079b079test
Serial.println("1st chr: "+String(int(_inputString2[0])));
Serial.println("2nd chr: "+String(int(_inputString2[1])));
  // fix if the string starts with newline
  String _inputString;
  if(_inputString2[0] == '\n' or _inputString2[0] == 0) {
    _inputString = _inputString2.substring(1);
    Serial.println("Newline Fix executed: "+_inputString);
  }
  else {
    _inputString = _inputString2;
  }

  if(_inputString[0] == 'P') {
  // All program stack manipulation codes 
  // Samples here:
  // 
    if(_inputString[1] == 's') {
    //stack size in bytes
      Serial.println("stack size: "+String(sizeof(mainProgramString)));
    }
    if(_inputString[1] == 'w') {
    //write into stack
      int _offset = _inputString.substring(2,5).toInt();
      String _data = _inputString.substring(5);
      Serial.println("write to slot: "+String(_offset));
      mainProgramString[_offset] = _data;
      // write non volatile memory as well
      EEPROM.writeString(_offset, _data);
    }
    if(_inputString[1] == 'p') {
    //print contents of the specified memory location
      int _offset = _inputString.substring(2,5).toInt();
      Serial.println("Contents of ["+String(_offset)+"] : "+String(mainProgramString[_offset]));
    }
    if(_inputString[1] == 'e') {
    //execute all commands in the specified memory location
      //execute current command line
      int _offset = _inputString.substring(2,5).toInt();
      //add all commands sepprated by ;
      String stillToBeExecuted = mainProgramString[_offset];
      int pos = stillToBeExecuted.indexOf(';');
      
      if(pos>0) {
        while(1) {
          pos = stillToBeExecuted.indexOf(';');
          if(pos > 0) {
            String toExec = stillToBeExecuted.substring(0,pos);
            stillToBeExecuted = stillToBeExecuted.substring(pos+1);
            Serial.println("Executed ["+String(_offset)+"]["+String(pos)+"] : "+toExec);
            parseDrawInput(toExec);    
          } else {
            Serial.println("Executed ["+String(_offset)+"][-1] : "+stillToBeExecuted);
            parseDrawInput(stillToBeExecuted);
            break;
          }
        
        }
      }
      else {
        Serial.println("Executed ["+String(_offset)+"] : ");
        parseDrawInput(stillToBeExecuted);
      }
    }
    if(_inputString[1] == 'r') {

      Serial.println("Started Reading");
      Serial.println(EEPROM.readString(0));
      
    }
  }
  
  if(_inputString[0] == 'B') {
    int _x,_y,_width, _height, _r, _g, _b;
    _x = _inputString.substring(1,5).toInt();
    _y = _inputString.substring(5,9).toInt();
    _width = _inputString.substring(9,13).toInt();
    _height = _inputString.substring(13,17).toInt();
    _r = _inputString.substring(17,21).toInt();
    _g = _inputString.substring(21,25).toInt();
    _b = _inputString.substring(25,29).toInt();
    Serial.println("Draw Box:\n    x: "+String(_x)+" y: "+String(_y)+" width: "+String(_width)+" height: "+String(_height)+ " Color: ("+String(_r)+","+String(_g)+","+String(_b)+")");
    
    lcd.fillRect(_x,_y,_width,_height,lcd.color888(_r,_g,_b));
  }
  if(_inputString[0] == 'T') {
    int _x,_y,_align, _font, _rf, _gf, _bf, _rb, _gb, _bb;
    String _val;
    _x = _inputString.substring(1,5).toInt();
    _y = _inputString.substring(5,9).toInt();
    _align = _inputString.substring(9,11).toInt();
    _font = _inputString.substring(11,13).toInt();
    _rf = _inputString.substring(13,17).toInt();
    _gf = _inputString.substring(17,21).toInt();
    _bf = _inputString.substring(21,25).toInt();
    _rb = _inputString.substring(25,29).toInt();
    _gb = _inputString.substring(29,33).toInt();
    _bb = _inputString.substring(33,36).toInt();
    _val = _inputString.substring(36);
    Serial.println("Draw Text:\n    x: "+String(_x)+" y: "+String(_y)+" align: "+String(_align)+" Font: "+String(_font)+" Fg Color: ("+String(_rf)+","+String(_gf)+","+String(_bf)+")  Bg Color: ("+String(_rb)+","+String(_gb)+","+String(_bb)+")");

    lcd.setTextFont(_font);
    //https://github.com/lovyan03/LovyanGFX/blob/5cfb85d2843ad13cea571af86154d26551991ca8/examples/HowToUse/3_fonts/3_fonts.ino#L126
    lcd.setTextDatum(_align);
    lcd.setTextColor(lcd.color888(_rf,_gf,_bf), lcd.color888(_rb,_gb,_bb));
    lcd.drawString(_val, _x, _y);
  }
  
} 

void loop()
{
  if(tick_update==true) {
    tick_update=false;
    getEncoder();
    
    if (Serial.available() > 0) {
      String _lnin;
      _lnin = Serial.readString(); // read the incoming byte:
      parseDrawInput(_lnin);
    }

    if(udp.listen(6454)) {
      udp.onPacket([](AsyncUDPPacket packet) {
        //Serial.write(packet.data(),packet.length());
        String _data = (const char*)packet.data();
        parseDrawInput(_data);
      });
    }

    
  }
  /*GroupSelector.reDraw();
  propIntensity.reDraw();
  propSaturation.reDraw();
  propHue.reDraw();
  propTemperature.reDraw();
  propFx.reDraw();
  propViewers.reDraw();
  propButton.reDraw();*/
 // lcd.pushImage(0,0, 480, 320, img1_map);
}

bool detectButtonTouch(int x1, int y1, int x2, int y2){
  if(x1>x2 || y1>y2){
    return 0;
  }
  if(lcd.getTouch(&x, &y)){
    if(x1<=x && x<=x2 && y1<=y && y<=y2){
      return 1;
    }
    X=x;
    Y=y;
  }
  return 0;
}
void drawLabels(){
  lcd.setCursor(0,52);
  lcd.printf("Button 1");
}
