#ifndef _MDISPLAY_H_
#define _MDISPLAY_H_

class Adafruit_ST7735;

class MDisplay
{
  public:
    MDisplay();
    ~MDisplay();

    void initLCD();
    void runDisplay(float u, float i, float celsius, int time, float ah, bool ap);
    void showRealVoltage();
    void showRealCurrent();



  private:
    Adafruit_ST7735 * ST7735 = nullptr;

};

#endif  // end _MDISPLAY_H_