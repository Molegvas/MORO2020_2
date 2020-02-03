/*
  Сonnection manager

*/

#ifndef _MCONNMNG_H_
#define _MCONNMNG_H_

class MTools;
class MBoard;
//class MOled;
  class MTft;

class MState;

class MConnect 
{
    public:
        MConnect(MTools * tools);

        void run();
        void delegateWork();

    private:
        MTools * Tools;
        MBoard * Board;
        //MOled * Oled;
          MTft  * Tft;

        MState * State = 0;

};

#endif  // !_MCONNMNG_H_