/*

  DrawSine.ino
  Draws a waveform on an Arduino nano with built in screen.
  Not just sinewaves!

  Uses Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

*/

#include <Arduino.h>
#include <U8g2lib.h>


enum wave_type {SINE, SQUARE, SAW, TRIANGLE};
const wave_type WAVE = SINE;

const bool DEBUG = false;

typedef struct coords
{
  int x;
  int y;
};
const int WAVE_LENGTH = 128; 
const int WAVE_COUNT = 7;
coords WavePoints[WAVE_LENGTH];
int ScreenWidth;
int ScreenHeight;

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, A5, A4, U8X8_PIN_NONE);  
// End of constructor list


void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

uint8_t y_offset = 0;

void draw(void) {
  //u8g2_prepare();
  coords LastCoord = {0, WavePoints[y_offset % ScreenWidth].x};
  for (int i = 0; i < ScreenWidth; i++)
  {
    coords CurrentCoord = {i, WavePoints[((i * WAVE_COUNT) + y_offset) % ScreenWidth].x};
    u8g2.drawLine(LastCoord.x, LastCoord.y, CurrentCoord.x, CurrentCoord.y);
    //u8g2.drawPixel(CurrentCoord.x, CurrentCoord.y);

    LastCoord = CurrentCoord;
    
  }
}

void setup(void) {
  
  if(DEBUG) Serial.begin(9600); //Debug mode. Serial port can only be used if not outputting data to scope 
  ScreenWidth = u8g2.getDisplayWidth();
  ScreenHeight = u8g2.getDisplayHeight();
  
  u8g2.begin();
  u8g2_prepare();
  u8g2.setCursor(0,3);
  u8g2.print("Screen Width:  ");
  u8g2.print(ScreenWidth);  // write something to the internal memory
  u8g2.setCursor(0,16);
  u8g2.print("Screen Height: ");
  u8g2.print(ScreenHeight);  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display

  if(WAVE == SINE) {
    for (int i = 0; i < ScreenWidth; i++)
    {
      WavePoints[i].x = (sin((2 * PI / ScreenWidth) * i) * ScreenHeight/2) + ScreenHeight/2;
      WavePoints[i].y = (cos((2 * PI / ScreenWidth) * i) * ScreenHeight/2) + ScreenHeight/2;
      if(DEBUG) Serial.println("X = " + String(WavePoints[i].x) + ", Y = " + String(WavePoints[i].x));
    }
  }
  else if(WAVE == SQUARE)
  {
    for (int i = 0; i < ScreenWidth; i++)
    {
      if (i < ScreenWidth / 2)
      {
        WavePoints[i].x = ScreenHeight - 1;
      }
      else 
      {
        WavePoints[i].x = 0;
      }
      
      WavePoints[i].y = WavePoints[i].x;
      if(DEBUG) Serial.println("X = " + String(WavePoints[i].x) + ", Y = " + String(WavePoints[i].x));
    }
  }
  else if(WAVE == TRIANGLE)
  {
    for (int i = 0; i < ScreenWidth; i++)
    {
      int s = ScreenWidth / (2 * ScreenHeight);
      if (i < ScreenWidth / 2)
      {
        WavePoints[i].x = i / s;
      }
      else 
      {
        WavePoints[i].x = (ScreenWidth - i) / s;
      }
      
      WavePoints[i].y = WavePoints[i].x;
      if(DEBUG) Serial.println("X = " + String(WavePoints[i].x) + ", Y = " + String(WavePoints[i].x));
    }
  }

  delay(3000); 
}

void loop(void) {
  // picture loop  
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
  
  // increase the state
  y_offset++;
  if ( y_offset >= ScreenWidth )
    y_offset = 0;

  // deley between each page
  //delay(10);

}
