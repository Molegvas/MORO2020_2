#ifndef _MTFT_H_
#define _MTFT_H_

//class U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI;
class Adafruit_ST7735;      //U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI;

class MTft
{
  public:
    MTft();
    ~MTft();

    void resetLCD();
    void runDisplay(float u, float i, float celsius, int time, float ah, bool ap);
    void showRealVoltage();
    void showRealCurrent();



  private:
    //U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI * U8g2 = nullptr;
    Adafruit_ST7735 * st7735 = nullptr;

};

#endif  // end _MTFT_H_