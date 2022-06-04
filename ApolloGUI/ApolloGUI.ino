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
  public:
    eButton(int _x, int _y, int _width, int _height, LGFX *_inLcd) {
      Xpos = _x;
      Ypos = _y;
      Width = _width;
      Height = _height;
      _lcd_ptr = _inLcd;
      
      }
    void reDraw() {
      if(_update==true) {
        //_lcd_ptr->fillRect(this->_Xpos,this->_Ypos,this->_Height,this->_Width,_lcd_ptr->color888(100,100,100));
        _lcd_ptr->fillRect(Xpos,Ypos,Width,Height,_lcd_ptr->color888(100,100,100));
        lcd.setTextColor(0x00FFFFU, 0xFF0000U);
        _lcd_ptr->drawString("Apollo0000", 10, 10);
        _update=false;
      }
    }
};

eButton GroupSelector(0,0,320,50,&lcd);

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
