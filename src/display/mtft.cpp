/*


*/

#include "mtft.h"
#include "board/mpins.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "utf8rus.h"
#include <SPI.h>
#include <Arduino.h>

  //SPIClass hspi(HSPI);


MTft::MTft()        //MOled::MOled()
{
    // The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp

    // U8g2 = new U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI( U8G2_R2, MPins::scl_pin,
    //                                                              MPins::sda_pin,
    //                                                              MPins::cs_pin,
    //                                                              MPins::dc_pin,
    //                                                              MPins::res_pin);  // 


  SPIClass hspi(HSPI);
  st7735 = new Adafruit_ST7735 (&hspi, MPins::cs_pin, MPins::dc_pin, MPins::res_pin);




    resetLCD();
}

MTft::~MTft()      //MOled::~MOled()
{
    //delete U8g2;
    delete st7735;
}

void MTft::resetLCD()
{
     st7735->cp437(true);

   //st7735->setRotation( 2 );         // 180 °С ( Alt+ 0 1 7 6 )
  // st7735->fillScreen(ST77XX_RED);  //BLACK);

   st7735->setFont();
   st7735->setTextColor(ST77XX_GREEN);
   //st7735->setTextSize(3);

   st7735->setCursor(2, 0);
   st7735->println("v");

//Serial.println("=======================init display");


}

void MTft::runDisplay(float u, float i, float celsius, int time, float ah, bool ap)
{
  // st7735->setTextWrap(false);
  // st7735->fillScreen(ST77XX_BLACK);
  // st7735->setFont();
  // st7735->setTextColor(ST77XX_GREEN);
  // st7735->setTextSize(3);
  // st7735->setCursor(2, 0);
  // st7735->print(u, 2);   st7735->println("v");
  // st7735->print(i, 2);   st7735->println("A");

  // st7735->setTextSize(2);
  // st7735->setCursor(2, 0);
  // st7735->println(utf8rus( (char *) " Настройки "));
//Serial.println("do display");

}

