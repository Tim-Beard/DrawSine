/*

  GraphicsTest.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

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
