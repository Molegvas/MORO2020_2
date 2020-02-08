#ifndef _MDISPLAY_H_
#define _MDISPLAY_H_

#include "stdio.h"
#include "stdint.h"

class Adafruit_ST7735;

class MDisplay
{
  public:
    MDisplay();
    ~MDisplay();

    void initLCD();
    void runDisplay(float u, float i, float celsius, int time, float ah, int percent, bool ap);

    void displayVoltage( float voltage );
    void displayCurrent( float current );
    void displayMode();    //( char *s );
    void displayHelp();    //( char *s );      
    void displayFulfill( int percent );
    void displayUpTime( unsigned long upSeconds );
    void displayAmpHours( float ah );
    void displayCelsius( float celsius );
    void displayLabel();   //( char *s );

    int  getMaxString();
    void getTextMode( char *s );
    void getTextHelp( char *s );
    void getTextLabel( char *s );


  private:
    Adafruit_ST7735 * ST7735 = nullptr;

    // Some ready-made 16-bit 5-6-5 my color settings:
    static constexpr uint16_t BLACK   = 0x0000;
    static constexpr uint16_t GRAY    = 0x39E7;
    static constexpr uint16_t WHITE   = 0xFFFF;
    static constexpr uint16_t RED     = 0xF800;
    static constexpr uint16_t GREEN   = 0x07E0;
    static constexpr uint16_t BLUE    = 0x001F;
    static constexpr uint16_t CYAN    = 0x07FF;
    static constexpr uint16_t MAGENTA = 0xF81F;
    static constexpr uint16_t YELLOW  = 0xFFE0;
    static constexpr uint16_t ORANGE  = 0xFC00;

    uint16_t displayBackroundColor = BLACK;

    struct MVoltage
    {
        // Параметры отображения напряжения
        static constexpr int16_t  cursorX   =  12;
        static constexpr int16_t  cursorY   =   0;
        static constexpr uint8_t  textSize  =   3;
        static constexpr uint16_t textColor = CYAN;
    };

    struct MCurrent
    {
        // Параметры отображения тока
        static constexpr int16_t  cursorX   =  12;
        static constexpr int16_t  cursorY   =  28;
        static constexpr uint8_t  textSize  =   3;
        static constexpr uint16_t textColor = CYAN;
    };

    struct MMode
    {
        // Параметры отображения режима
        static constexpr int16_t  cursorX   =   0;
        static constexpr int16_t  cursorY   =  56;
        static constexpr uint8_t  textSize  =   1;
        static constexpr uint16_t textColor = GREEN ;
    };

    struct MHelp
    {
        // Параметры отображения помощи
        static constexpr int16_t  cursorX   =   0;
        static constexpr int16_t  cursorY   =  66;
        static constexpr uint8_t  textSize  =   1;
        static constexpr uint16_t textColor = GREEN ;
    };

    struct MFilfull
    {
        // Параметры отображения %% выполнения
        static constexpr int16_t  minX      =  14;
        static constexpr int16_t  maxX      = 114;
        static constexpr int16_t  cursorY   =  78;
    };

    struct MTime
    {
        // Параметры отображения времени
        static constexpr int16_t  cursorX   = 10;
        static constexpr int16_t  cursorY   = 90;
        static constexpr uint8_t  textSize  =  2;
        static constexpr uint16_t textColor = YELLOW;
    };

    struct MAmpHours
    {
        // Параметры отображения ампер-часов
        static constexpr int16_t  cursorX   =  26;
        static constexpr int16_t  cursorY   = 110;
        static constexpr uint8_t  textSize  =   2;
        static constexpr uint16_t textColor = CYAN;
    };

    struct MCelsius
    {
        // Параметры отображения температуры
        static constexpr int16_t  cursorX   =  26;
        static constexpr int16_t  cursorY   = 130;
        static constexpr uint8_t  textSize  =   2;
        static constexpr uint16_t textColor = GREEN;
    };

    struct MLabel
    {
        // Параметры отображения лейбла
        static constexpr int16_t  cursorX   =   0;
        static constexpr int16_t  cursorY   = 150;
        static constexpr uint8_t  textSize  =   1;
        static constexpr uint16_t textColor = CYAN;
    };

    // declare size of working string buffers. Basic strlen("hhh:mm:ss") = 9
    static constexpr size_t MaxString = 22;   // Max 21 + 1 for TextSize 1 

    // the string being displayed on the ST7735 (initially empty)
    char oldVoltageString[ MaxString ]  = { 0 };
    char oldCurrentString[ MaxString ]  = { 0 };

    char newModeString[ MaxString ]     = { 0 };
    char oldModeString[ MaxString ]     = { 0 };

    char newHelpString[ MaxString ]     = { 0 };
    char oldHelpString[ MaxString ]     = { 0 };

    int  oldPercent                     =   0; 
    char oldTimeString[ MaxString ]     = { 0 };
    char oldAmpHoursString[ MaxString ] = { 0 };
    char oldCelsiusString[ MaxString ]  = { 0 };

    char newLabelString[ MaxString ]    = { 0 };
    char oldLabelString[ MaxString ]    = { 0 };

};

#endif  // !_MDISPLAY_H_