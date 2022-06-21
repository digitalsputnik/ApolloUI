/*
    Simple Touch Drawing sample for WT32-SC01
*/
#define LGFX_AUTODETECT     // Autodetect board
#define LGFX_USE_V1         // set to use new version of library

#include <LovyanGFX.hpp>    // main library
#include <FastLED.h>
#include <Wire.h>
#include <WireSlave.h>

#include <Preferences.h>

#include <WiFi.h>
#include "AsyncUDP.h"

//led control
#define NUM_LEDS 6
#define LED_DATA_PIN 4

static LGFX lcd;            // declare display variable

hw_timer_t * timer = NULL;

TwoWire i2c_p1 = TwoWire(0);

// Define the array of leds
CRGB leds[NUM_LEDS];
AsyncUDP udp;
Preferences preferences;


int count=0, node_events=0, encoder_offset=0, encoder_multplyer=1, encoder_min=0, encoder_max=100;
bool state=0, chk_wifi=true;
// Variables for touch x,y
static int32_t x,y;
int X,Y;

volatile bool tick_update=false;
int encoder_val, prev_encoder_val;

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
  Wire1.begin(25,5);
  WireSlave.begin(18, 19, 10);
  WireSlave.onRequest(I2CrequestEvent);
  lcd.init();
  lcd.setColorDepth(24);

  preferences.begin("ApolloNode", false);
  
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
  prev_encoder_val = encoder_val;

  initWiFi();

  parseDrawActionString("Pr000");
  parseDrawActionString("Pe000");

}

void I2CrequestEvent() {
  WireSlave.write('a');
  //WireSlave.write(0x33);
  Serial.println("I2C Event");
  }

void initWiFi() {
  parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000Connecting to WiFi ...");
  WiFi.mode(WIFI_STA);
  WiFi.begin("DS","SputnikulOn4Antenni");
  Serial.print("Connecting to WiFi ..");
  parseDrawActionString("Pe255");
  chk_wifi=true;
}

void i2cScan() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();
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
  Wire1.beginTransmission(0x40);
  Wire1.write(0x10);
  Wire1.endTransmission();

  //read encoder value
  Wire1.requestFrom(0x40, 2);
  for (int i = 0; i<2; i++) {
    data[i] = Wire1.read();
  }

  encoder_val = (signed short int)((data[0]) | (data[1]) << 8);

  if(prev_encoder_val!=encoder_val) {
    //if(node_events && 0x1) {
      node_events = node_events | 0x1;
      //parseDrawActionString("Pw064"+String(encoder_val/encoder_multplyer+encoder_offset));
      prev_encoder_val=encoder_val;
    //}
  }
}

void parseDrawActionString(String _inputString2) {
// Some simple strings to operate
// Clear Screen: B000 000w320h480r000g000b000
// dark cyan horizontal stripe: B000 239w320h002r000g030b100
// text test: T160 016a1f4r255g255b255r079g079b079test
//Serial.println("1st chr: "+String(int(_inputString2[0])));
//Serial.println("2nd chr: "+String(int(_inputString2[1])));
  // fix if the string starts with newline
  // TODO: push the whitespace removal in to save functionality NB! Spaces and newlines in data strings must be preservered
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
      // cut out tailing new line from prompt entry
      String _data;
      if(_inputString[_inputString.length()] == '/n') {
        _data = _inputString.substring(5,_inputString.length()-1);
      }
      else {
        _data = _inputString.substring(5);
      }
      Serial.println("last char: "+String(_inputString[_inputString.length()-1]));
      Serial.println("write to slot: "+String(_offset));
      mainProgramString[_offset] = _data;
      // write non volatile memory as well
      char _offsetString[3];
      itoa(_offset, _offsetString, 10);
      preferences.putString(_offsetString, _data);
    }
    if(_inputString[1] == 'p') {
    //print contents of the specified memory location
      int _offset = _inputString.substring(2,5).toInt();
      Serial.println("Contents of ["+String(_offset)+"] : "+String(mainProgramString[_offset]));
    }
    if(_inputString[1] == 'r') {
    //read data from EEPROM
      int _offset = _inputString.substring(2,5).toInt();
      char _offsetString[3];
      itoa(_offset, _offsetString, 10);      
      Serial.println("read slot: "+String(_offset));
      mainProgramString[_offset] = preferences.getString(_offsetString,"");
    }
    if(_inputString[1] == 'm') {
    //merge 2 strings slot1base slot2insert offset char_count
      int _slot1base = _inputString.substring(2,5).toInt();
      int _slot2insert = _inputString.substring(5,9).toInt();
      int _offset = _inputString.substring(9,13).toInt();
      int _count = mainProgramString[_slot1base].length()-_offset;
      
      Serial.println("Copy ["+String(_slot1base)+"] -> ["+String(_slot2insert)+"] with offset: "+String(_count));
      parseDrawActionString("Pw064"+mainProgramString[_slot1base].substring(0,_count)+mainProgramString[_slot2insert]+mainProgramString[_slot1base].substring(_count));    
      
    }
    if(_inputString[1] == 'c') {
    //copy one slot to another slot2insert offset char_count
      int _slot1from = _inputString.substring(2,5).toInt();
      int _slot2to = _inputString.substring(5,9).toInt();

      mainProgramString[_slot2to] = mainProgramString[_slot1from];
      
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
            parseDrawActionString(toExec);    
          } else {
            Serial.println("Executed ["+String(_offset)+"][-1] : "+stillToBeExecuted);
            parseDrawActionString(stillToBeExecuted);
            break;
          }
        
        }
      }
      else {
        Serial.println("Executed ["+String(_offset)+"] : ");
        parseDrawActionString(stillToBeExecuted);
      }
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
  if(_inputString[0] == 'R') {
    int _x,_y,_width, _height, _r, _g, _b;
    _x = _inputString.substring(1,5).toInt();
    _y = _inputString.substring(5,9).toInt();
    _width = _inputString.substring(9,13).toInt();
    _height = _inputString.substring(13,17).toInt();
    _r = _inputString.substring(17,21).toInt();
    _g = _inputString.substring(21,25).toInt();
    _b = _inputString.substring(25,29).toInt();
    Serial.println("Draw Rectangle:\n    x: "+String(_x)+" y: "+String(_y)+" width: "+String(_width)+" height: "+String(_height)+ " Color: ("+String(_r)+","+String(_g)+","+String(_b)+")");
    
    lcd.drawRect(_x,_y,_width,_height,lcd.color888(_r,_g,_b));
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
  if(_inputString[0] == 'x') {
    if(_inputString[1] == '1') {
        parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000chk_wifi: "+String(chk_wifi));
        parseDrawActionString("Pe255");
    }
    if(_inputString[1] == '2') {
        parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000WL_CONNECTED: "+String(WL_CONNECTED)+":"+String(WiFi.status()));
        parseDrawActionString("Pe255");
    }
    if(_inputString[1] == '3') {
        String _local_ip = WiFi.localIP().toString();
        Serial.println(_local_ip);
        //parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000IP: "+WiFi.localIP().toString());
        //parseDrawActionString("Pe255");
    }
    if(_inputString[1] == '4') {
        parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000false: "+String(false) + " true: "+String(true));
        parseDrawActionString("Pe255");
        chk_wifi=true;
    }
    if(_inputString[1] == '5') {
        //Set encoder value multiplyer
        int _multiplyer = _inputString.substring(2,5).toInt();
        encoder_multplyer = _multiplyer;
    }
    if(_inputString[1] == '6') {
        //Set 0 Offset
        int _offset = _inputString.substring(2,5).toInt();
        int _prev_val = mainProgramString[_offset].toInt();
        encoder_offset = (_prev_val/encoder_multplyer)-(encoder_val/encoder_multplyer);
    }
    if(_inputString[1] == '7') {
        //Generate new value and print it on display
        int _offset = _inputString.substring(2,5).toInt();
        int _prev_val = mainProgramString[_offset].toInt();
        String _calculated_value = String((encoder_val+encoder_offset)*encoder_multplyer);
        mainProgramString[_offset] = _calculated_value;
        Serial.println("Caculated encoder value to: "+String(_calculated_value) + " -> " + String(_offset)+ " from: "+String(_prev_val));
    }
    if(_inputString[1] == '8') {
        Serial.println("Encoder: "+String(encoder_val)+"/"+String(encoder_multplyer)+"/"+String(encoder_offset));
    }
    
    if(_inputString[1] == 'x') {
        ESP.restart();
    }
    
  }
  if(_inputString.substring(0,9) == "stringbuf") {
    Serial.println("last string: "+mainProgramString[64]);
  }
  
} 

void loop()
{
  WireSlave.update();
  if(tick_update==true) {
    tick_update=false;
    getEncoder();
    
    if (Serial.available() > 0) {
      String _lnin;
      _lnin = Serial.readString(); // read the incoming byte:
      parseDrawActionString(_lnin);
    }

    if(udp.listen(6454)) {
      udp.onPacket([](AsyncUDPPacket packet) {
        //Serial.write(packet.data(),packet.length());
        String _data = (const char*)packet.data();
        parseDrawActionString(_data);
      });
    }

    if(chk_wifi==true) {
      if (WiFi.status() == WL_CONNECTED) {
        parseDrawActionString("Pw255B000 460w320h020r000g000b000;T160 460a1f2r100g100b100r000g000b000"+WiFi.localIP().toString());
        parseDrawActionString("Pe255");
        chk_wifi = false;
      }
    }
    if(node_events != 0x0) {
      // every event gets consecutive byte so it would be possible to register more than one event, all bytes get reset after handeling
      if(node_events && 0x1) {
        //encoder event
        parseDrawActionString("Pe016");
      }
      node_events = 0;
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
