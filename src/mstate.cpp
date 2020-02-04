#include "mstate.h"
#include "mtools.h"

MState::MState(MTools * Tools) :
  Tools(Tools),
  Board(Tools->Board),
  //Oled(Tools->Oled),
    Display(Tools->Display),
  Keyboard(Tools->Keyboard) {}
