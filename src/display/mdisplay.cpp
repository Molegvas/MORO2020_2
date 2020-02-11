/*
    Дисплей TFT 1.8" 4-SPI 128*160
    HSPI
    2020.02.09
    OlMoro
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
    ST7735->setRotation( 2 );           // 180 °С ( Alt+ 0 1 7 6 )
    ST7735->setFont();
    ST7735->setTextColor(ST77XX_GREEN);
    displayBackroundColor = BLACK;
    ST7735->fillScreen( displayBackroundColor );
    ST7735->setTextWrap(false);
    ST7735->setFont();
}

//void MDisplay::runDisplay(float u, float i, float celsius, int time, float ah, int percent, bool ap)
void MDisplay::runDisplay( float celsius, bool ap)
{
    displayVoltage();
    displayCurrent();
    displayMode();
    displayHelp();      
    displayFulfill();
    displayDuration();
    displayAmphours();

    displayCelsius( celsius );
    displayLabel();
  // пример для русификации  
  //ST7735->setCursor(0, 80);
  //ST7735->println(utf8rus( (char *) " Настройки "));
}

void MDisplay::voltage( float voltage, int n )
{
    if( n == 2 ) sprintf( newVoltageString, "%2.2fv", voltage );
    else         sprintf( newVoltageString, "%2.1fv", voltage );

}

// Пока без центровки в строке
void MDisplay::displayVoltage()
{
    if (strcmp( newVoltageString, oldVoltageString) != 0) 
    {
        ST7735->setTextSize( MVoltage::textSize );
        ST7735->setCursor( MVoltage::cursorX, MVoltage::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldVoltageString );
        ST7735->setCursor( MVoltage::cursorX, MVoltage::cursorY );
        ST7735->setTextColor( MVoltage::textColor );
        ST7735->print( newVoltageString );
        strcpy( oldVoltageString, newVoltageString );
    }
}

void MDisplay::current( float current, int n )
{
    if( n == 2 ) sprintf( newCurrentString, "%2.2fA", current );
    else         sprintf( newCurrentString, "%2.1fA", current );
}

void MDisplay::displayCurrent()
{
    if (strcmp( newCurrentString, oldCurrentString) != 0) 
    {
        ST7735->setTextSize( MCurrent::textSize );
        ST7735->setCursor( MCurrent::cursorX, MCurrent::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldCurrentString );
        ST7735->setCursor( MCurrent::cursorX, MCurrent::cursorY );
        ST7735->setTextColor( MCurrent::textColor );
        ST7735->print( newCurrentString );
        strcpy( oldCurrentString, newCurrentString );
    }
}

void MDisplay::displayMode()
{
    if (strcmp( newModeString, oldModeString) != 0) 
    {
        ST7735->setTextSize( MMode::textSize );
        ST7735->setCursor( MMode::cursorX, MMode::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldModeString );
        ST7735->setCursor( MMode::cursorX, MMode::cursorY );
        ST7735->setTextColor( MMode::textColor );
        ST7735->print( newModeString );
        strcpy( oldModeString, newModeString );
    }
}

void MDisplay::displayHelp()
{
    if (strcmp( newHelpString, oldHelpString) != 0) 
    {
        ST7735->setTextSize( MHelp::textSize );
        ST7735->setCursor( MHelp::cursorX, MHelp::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldHelpString );
        ST7735->setCursor( MHelp::cursorX, MHelp::cursorY );
        ST7735->setTextColor( MHelp::textColor );
        ST7735->print( newHelpString );
        strcpy( oldHelpString, newHelpString );
    }
}

void MDisplay::fulfill( int val, uint16_t rgb )
{
    percent = val;
    color   = rgb;
}


void MDisplay::displayFulfill()
{
    if ( percent != oldPercent ) 
    {
        int x = percent + MFilfull::minX;

        for (int i = 0; i <= 5; i++)
        {
            int y = MFilfull::cursorY + i;

            ST7735->drawLine( MFilfull::minX, y, x, y, color );
            ST7735->drawLine( x, y, MFilfull::maxX, y, GRAY ); 
        } 

        oldPercent = percent;
    }
}

          //  Tools->getChargeTimeCounter(),
void MDisplay::duration( int duration, int _plan )     // unsigned long ??
{
    upSeconds = duration;
    plan = _plan;
}

//void MDisplay::displayUpTime( unsigned long upSeconds )
void MDisplay::displayDuration()
{
    char newTimeString[ MDisplay::MaxString ] = { 0 };

    if( plan == SEC )
    {
        upSeconds = upSeconds % 86400;
        unsigned long hours = upSeconds / 3600;
        upSeconds = upSeconds % 3600;
        unsigned long minutes = upSeconds / 60;
        upSeconds = upSeconds % 60;
    //    char newTimeString[ MDisplay::MaxString ] = { 0 };
        sprintf( newTimeString, "%03lu:%02lu:%02lu", hours, minutes, upSeconds );
    }
    else
    {
        sprintf( newTimeString, "%3d h", upSeconds );
    }

    if (strcmp( newTimeString, oldTimeString) != 0) 
    {
        ST7735->setTextSize( MTime::textSize );
        ST7735->setCursor( MTime::cursorX, MTime::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldTimeString );
        ST7735->setCursor( MTime::cursorX, MTime::cursorY );
        ST7735->setTextColor( MTime::textColor );
        ST7735->print( newTimeString );
        strcpy( oldTimeString, newTimeString );
    }
}

void MDisplay::amphours( float val )
{
    ah = val;
} 

void MDisplay::displayAmphours()       //( float ah )
{
    char newAmpHoursString[ MDisplay::MaxString ] = { 0 };

    sprintf( newAmpHoursString, "%3.1f Ah", ah );
    if (strcmp( newAmpHoursString, oldAmpHoursString) != 0) 
    {
        ST7735->setTextSize( MAmpHours::textSize );
        ST7735->setCursor( MAmpHours::cursorX, MAmpHours::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldAmpHoursString );
        ST7735->setCursor( MAmpHours::cursorX, MAmpHours::cursorY );
        ST7735->setTextColor( MAmpHours::textColor );
        ST7735->print( newAmpHoursString );
        strcpy( oldAmpHoursString, newAmpHoursString );
    }
}

// void MDisplay::celsius()
// {
//     float celsius = Board->Overseer->getCelsius(),
//     sprintf( newCelsiusString, "%3.1f C", celsius );
// }

void MDisplay::displayCelsius( float celsius )
{
    char newCelsiusString[ MDisplay::MaxString ] = { 0 };

    sprintf( newCelsiusString, "%3.1f C", celsius );
    if (strcmp( newCelsiusString, oldCelsiusString) != 0) 
    {
        ST7735->setTextSize( MCelsius::textSize );
        ST7735->setCursor( MCelsius::cursorX, MCelsius::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldCelsiusString );
        ST7735->setCursor( MCelsius::cursorX, MCelsius::cursorY );
        ST7735->setTextColor( MCelsius::textColor );
        ST7735->print( newCelsiusString );
        strcpy( oldCelsiusString, newCelsiusString );
    }
}

void MDisplay::displayLabel()
{
    if (strcmp( newLabelString, oldLabelString) != 0) 
    {
        ST7735->setTextSize( MLabel::textSize );
        ST7735->setCursor( MLabel::cursorX, MLabel::cursorY );
        ST7735->setTextColor( displayBackroundColor );
        ST7735->print( oldLabelString );
        ST7735->setCursor( MLabel::cursorX, MLabel::cursorY );
        ST7735->setTextColor( MLabel::textColor );
        ST7735->print( newLabelString );
        strcpy( oldLabelString, newLabelString );
    }
}

void MDisplay::getTextMode( char *s )
{
    strcpy( newModeString, s );
}

void MDisplay::getTextHelp( char *s )
{
    strcpy( newHelpString, s );
}

void MDisplay::getTextLabel( char *s )
{
    strcpy( newLabelString, s );
}

void MDisplay::progessBarOff() { fulfill( 0, GRAY ); }
void MDisplay::progessBarExe( uint16_t rgb )
{
    static int x = 0;
    x += 4;
    if (x >= 100 ) x = 0;
    fulfill( x, rgb );
}
void MDisplay::progessBarStop() { fulfill( 100, RED ); }
