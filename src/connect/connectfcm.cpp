/*
  Сonnections state machine 

*/

#include "connectfsm.h"
#include "fsbrowser.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/moverseer.h"
//#include "display/mtft.h"
#include <Arduino.h>

namespace ConnectFsm
{
// Initialization SPIFFS, 
    MState * MInit::fsm()
    {
        initFSBrowser();
        return new MExe(Tools);
    };

// Running
    MState * MExe::fsm()
    {
        runFSBrowser( Board->getRealVoltage(), 
              Board->getRealCurrent(), 
              Board->Overseer->getCelsius(), 
              Tools->getAhCharge(), 
              Board->Overseer->getFanPwm() );
        return this;
    };

};
