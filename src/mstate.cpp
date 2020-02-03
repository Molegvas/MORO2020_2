#include "mstate.h"
#include "mtools.h"

MState::MState(MTools * Tools) :
  Tools(Tools),
  Board(Tools->Board),
  //Oled(Tools->Oled),
    Tft(Tools->Tft),
  Keyboard(Tools->Keyboard) {}
