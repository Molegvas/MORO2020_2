/*
  Сonnection manager

*/

#ifndef _MCONNMNG_H_
#define _MCONNMNG_H_

class MTools;
class MBoard;
//class MOled;
  class MDisplay;

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
          MDisplay * Display;

        MState * State = 0;

};

#endif  // !_MCONNMNG_H_