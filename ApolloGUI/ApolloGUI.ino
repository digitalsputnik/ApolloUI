/*
    Simple Touch Drawing sample for WT32-SC01
*/
#define LGFX_AUTODETECT     // Autodetect board
#define LGFX_USE_V1         // set to use new version of library

#include <LovyanGFX.hpp>    // main library
//#include <Adafruit_ImageReader.h>

static LGFX lcd;            // declare display variable
static LGFX_Sprite sprite(&lcd);
int count=0;
bool state=0;
// Variables for touch x,y
static int32_t x,y;
int X,Y;

//Adafruit_Image img;

class eButton {
  private:
    int Xpos,Ypos,Width,Height;
    LGFX *_lcd_ptr;
    bool _update = true;
    String Type, Val;
  public:
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
          lcd.drawString(Val, 290, Ypos+10);          
        }
        _update=false;
      }
    }
};

eButton GroupSelector(0,0,320,54,&lcd,"G","Apollo0000");
eButton propIntensity(10,64,300,40,&lcd,"Intenisty","80%");
eButton propSaturation(10,114,300,40,&lcd,"Saturation","0%");
eButton propHue(10,164,300,40,&lcd,"Hue","0%");
eButton propTemperature(10,214,300,40,&lcd,"Temperature","5600K");
eButton propFx(10,264,300,40,&lcd,"Fx","0%");
eButton propViewers(10,314,300,40,&lcd,"Manage viewers","");
eButton propButton(160,430,150,40,&lcd,"B","Setup");

void setup(void)
{
  Serial.begin(115200);
  lcd.init();
  lcd.setColorDepth(24);

  // Setting display to landscape
  //if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 1);

}

void loop()
{

  GroupSelector.reDraw();
  propIntensity.reDraw();
  propSaturation.reDraw();
  propHue.reDraw();
  propTemperature.reDraw();
  propFx.reDraw();
  propViewers.reDraw();
  propButton.reDraw();
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
