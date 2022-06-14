/*
    Simple Touch Drawing sample for WT32-SC01
*/
#define LGFX_AUTODETECT     // Autodetect board
#define LGFX_USE_V1         // set to use new version of library

#include <LovyanGFX.hpp>    // main library
#include <FastLED.h>
#include <Wire.h>

#include <EEPROM.h>

//led control
#define NUM_LEDS 6
#define LED_DATA_PIN 4

static LGFX lcd;            // declare display variable

hw_timer_t * timer = NULL;


// Define the array of leds
CRGB leds[NUM_LEDS];



int count=0;
bool state=0;
// Variables for touch x,y
static int32_t x,y;
int X,Y;

volatile bool tick_update=false;
int intensity_val, prev_intensity_val;

//Adafruit_Image img;

class eButton {
  private:
    int Xpos,Ypos,Width,Height;
    LGFX *_lcd_ptr;
    
    String Type, Val;
  public:
    bool _update = true;
    eButton(int _x, int _y, int _width, int _height, LGFX *_inLcd, String _type, String _val) {
      Xpos = _x;
      Ypos = _y;
      Width = _width;
      Height = _height;
      _lcd_ptr = _inLcd;
      Type = _type;
      Val = _val;
      
      }
    void reDraw() {
      if(_update==true) {
        //_lcd_ptr->fillRect(this->_Xpos,this->_Ypos,this->_Height,this->_Width,_lcd_ptr->color888(100,100,100));
        lcd.setTextFont(4);
   
        
        
        if(Type == "G") {
          lcd.fillRect(Xpos,Ypos,Width,Height,_lcd_ptr->color888(46,46,46));
          lcd.fillRect(Xpos+10,Ypos+10,Width-20,Height-20,_lcd_ptr->color888(79,79,79));
       
        
          lcd.setTextDatum(textdatum_t::top_center);
          lcd.setTextColor(_lcd_ptr->color888(255,255,255), _lcd_ptr->color888(79,79,79));
          lcd.drawString(Val, 160, Ypos+16);
          lcd.setTextDatum(textdatum_t::top_left);
          lcd.setTextColor(_lcd_ptr->color888(255,213,46), _lcd_ptr->color888(79,79,79));
          lcd.drawString("<", 30, Ypos+16);
          lcd.setTextDatum(textdatum_t::top_right);
          lcd.drawString(">", 290, Ypos+16);
        }
        else if (Type == "B") {
          lcd.fillRect(Xpos,Ypos,Width,Height,_lcd_ptr->color888(46,46,46));

          lcd.setTextDatum(textdatum_t::top_center);
          lcd.setTextColor(_lcd_ptr->color888(255,213,46), _lcd_ptr->color888(46,46,46));
          lcd.drawString(Val, 235, Ypos+10);
        }
        else {
          lcd.fillRect(Xpos,Ypos,Width,Height,_lcd_ptr->color888(46,46,46));
        
          lcd.setTextDatum(textdatum_t::top_left);
          lcd.setTextColor(_lcd_ptr->color888(255,213,46), _lcd_ptr->color888(46,46,46));
          lcd.drawString(Type, 30, Ypos+10);
          lcd.setTextDatum(textdatum_t::top_right);
          lcd.setTextColor(_lcd_ptr->color888(79,79,79), _lcd_ptr->color888(46,46,46));
          if(Type == "Intensity") {
            lcd.drawString(String(intensity_val)+"%", 290, Ypos+10);
          }
          else {
            lcd.drawString(Val, 290, Ypos+10);          
          }
        }
        _update=false;
      }
    }
};

String startupString[] = {  
                            // Group Box
                            "B000 000w320h054r046g046b046",
                            "B010 010w300h034r079g079b079",
                              //text
                            "T160 016a1r255g255b255r079g079b079Apollo0189",
                            "T030 016a0r255g255b255r079g079b079<",
                            "T290 016a2r255g255b255r079g079b079>",

                            //int
                            "B010 064w300h040r046g046b046",
                            "T030 074a0r255g213b046r046g046b046Intensity",
                            "T290 074a2r079g079b079r046g046b046100%",
                            
                            //temp
                            "B010 114w300h040r046g046b046",
                            "T030 124a0r255g213b046r046g046b046Temperature",
                            "T290 124a2r079g079b079r046g046b0465600K",

                            //sat
                            "B010 164w300h040r046g046b046",
                            "T030 174a0r255g213b046r046g046b046Saturation",
                            "T290 174a2r079g079b079r046g046b0460%",
                            
                            //hue
                            "B010 214w300h040r046g046b046",
                            "T030 224a0r255g213b046r046g046b046Hue",
                            "T290 224a2r079g079b079r046g046b0460%",   

                            //FX
                            "B010 264w300h040r046g046b046",
                            "T030 274a0r255g213b046r046g046b046Fx",
                            "T290 274a2r079g079b079r046g046b040%",

                            //Manage Viewers
                            "B010 314w300h040r046g046b046",
                            "T030 324a0r255g213b046r046g046b046Manage Viewers"
                            };


String mainProgramString[255];
                            
eButton GroupSelector(0,0,320,54,&lcd,"G","Apollo0189");
eButton propIntensity(10,64,300,40,&lcd,"Intensity","80%");
eButton propSaturation(10,114,300,40,&lcd,"Saturation","0%");
eButton propHue(10,164,300,40,&lcd,"Hue","0%");
eButton propTemperature(10,214,300,40,&lcd,"Temperature","5600K");
eButton propFx(10,264,300,40,&lcd,"Fx","0%");
eButton propViewers(10,314,300,40,&lcd,"Manage viewers","");
eButton propButton(160,430,150,40,&lcd,"B","Setup");



void tick5ms() {
  tick_update=true;
  }

void setup(void)
{
    
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &tick5ms, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  
  Serial.begin(115200);
  Wire.begin(25,5);
  lcd.init();
  lcd.setColorDepth(24);

  if (!EEPROM.begin(64)) {
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

  for(int i = 0; i<22; i++) {
    parseDrawInput(startupString[i]);
  }

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
    propIntensity._update=true;
    prev_intensity_val=intensity_val;
  }
}

void parseDrawInput(String _inputString) {
// Some simple strings to operate
// Clear Screen: B000 000w320h480r000g000b000
// dark cyan horizontal stripe: B000 239w320h002r000g030b100
// text test: T160 016a1r255g255b255r079g079b079test
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
    if(_inputString[1] == 'x') {
      
      Serial.println("Main stack stored in flash ['/screen001.sc']");
      EEPROM.writeString(0, mainProgramString[1]);
      
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
    int _x,_y,_align, _rf, _gf, _bf, _rb, _gb, _bb;
    String _val;
    _x = _inputString.substring(1,5).toInt();
    _y = _inputString.substring(5,9).toInt();
    _align = _inputString.substring(9,11).toInt();
    _rf = _inputString.substring(11,15).toInt();
    _gf = _inputString.substring(15,19).toInt();
    _bf = _inputString.substring(19,23).toInt();
    _rb = _inputString.substring(23,27).toInt();
    _gb = _inputString.substring(27,31).toInt();
    _bb = _inputString.substring(31,34).toInt();
    _val = _inputString.substring(34);
    Serial.println("Draw Text:\n    x: "+String(_x)+" y: "+String(_y)+" align: "+String(_align)+" Fg Color: ("+String(_rf)+","+String(_gf)+","+String(_bf)+")  Bg Color: ("+String(_rb)+","+String(_gb)+","+String(_bb)+")");

    lcd.setTextFont(4);
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
