/*
  Colors:
      ST77XX_BLACK 0x0000
      ST77XX_WHITE 0xFFFF
      ST77XX_RED 0xF800
      ST77XX_GREEN 0x07E0
      ST77XX_BLUE 0x001F
      ST77XX_CYAN 0x07FF
      ST77XX_MAGENTA 0xF81F
      ST77XX_YELLOW 0xFFE0
      ST77XX_ORANGE 0xFC00
*/

#include "mdisplay.h"
#include "board/mpins.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "utf8rus.h"
#include <SPI.h>
#include <Arduino.h>

SPIClass hspi(HSPI);


MDisplay::MDisplay()
{
  ST7735 = new Adafruit_ST7735 (&hspi, MPins::cs_pin, MPins::dc_pin, MPins::res_pin);

  initLCD();
}

MDisplay::~MDisplay()
{
    delete ST7735;
}

void MDisplay::initLCD()
{
  ST7735->cp437(true);
  ST7735->initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  ST7735->setRotation( 2 );         // 180 °С ( Alt+ 0 1 7 6 )
  ST7735->setFont();
  ST7735->setTextColor(ST77XX_GREEN);
  ST7735->fillScreen(ST77XX_BLACK);
}

void MDisplay::runDisplay(float u, float i, float celsius, int time, float ah, bool ap)
{
  ST7735->setTextWrap(false);
  ST7735->fillScreen(ST77XX_BLACK);
  ST7735->setFont();

  ST7735->setTextSize(3);
  ST7735->setTextColor(ST77XX_CYAN);

    ST7735->setCursor(12, 0);
    ST7735->print(u, 2);   ST7735->print("v");

    ST7735->setCursor(20, 28);
    ST7735->print(i, 2);   ST7735->print("A");

  ST7735->setTextSize(2);

    ST7735->setCursor(5, 56);
    ST7735->setTextColor(ST77XX_GREEN);
    ST7735->print("CCCVcharge");


    ST7735->drawLine(2, 79, 80, 79, ST77XX_GREEN);
    ST7735->drawLine(2, 80, 80, 80, ST77XX_GREEN);
    ST7735->drawLine(2, 81, 80, 81, ST77XX_GREEN);

    ST7735->drawLine(81, 79, 125, 79, ST77XX_RED);
    ST7735->drawLine(81, 80, 125, 80, ST77XX_RED);
    ST7735->drawLine(81, 81, 125, 81, ST77XX_RED);


    ST7735->setCursor(5, 90);
    ST7735->setTextColor(ST77XX_YELLOW);
    ST7735->print(" 120:01:22");

    ST7735->setCursor(40, 110);
    ST7735->setTextColor(ST77XX_CYAN);
    ST7735->print(ah, 1);   ST7735->print(" Ah");

    ST7735->setCursor(40, 130);
    ST7735->setTextColor(ST77XX_GREEN);
    ST7735->print(celsius, 1);   ST7735->print(" C");

  ST7735->setTextSize(1);
    ST7735->setTextColor(ST77XX_CYAN);
    ST7735->setCursor(15, 150);
    ST7735->print("olmoro     bal");
    ST7735->setTextColor(ST77XX_RED);
    ST7735->print("sat");

  //ST7735->setCursor(0, 80);
  //ST7735->println(utf8rus( (char *) " Настройки "));


}

