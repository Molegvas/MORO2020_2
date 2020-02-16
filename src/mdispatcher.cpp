#include "mdispatcher.h"
#include "nvs.h"
#include "mtools.h"
#include "board/mboard.h"
#include "measure/mkeyboard.h"
#include "display/mdisplay.h"

#include <string>

#include "modes/optionsfsm.h"
#ifdef TEMPLATE_ENABLE
  #include "modes/templatefsm.h"
#endif
#ifdef DC_SUPPLY_ENABLE
  #include "modes/dcsupplyfsm.h"
#endif
#ifdef PULSE_GEN_ENABLE
  #include "modes/pulsegenfsm.h"
#endif
#ifdef CCCV_CHARGE_ENABLE
  #include "modes/cccvfsm.h"
#endif
#ifdef PULSE_CHARGE_ENABLE           
  #include "modes/exchargerfsm.h"
#endif
#ifdef RECOVERY_ENABLE
  #include "modes/recoveryfsm.h"
#endif
#ifdef STORAGE_ENABLE
  #include "modes/storagefsm.h"
#endif
#ifdef DEVICE_ENABLE
  #include "modes/devicefsm.h"
#endif
#include "modes/servicefsm.h"
#include "Arduino.h"


MDispatcher::MDispatcher(MTools * tools) :
Tools(tools), Board(tools->Board), Display(tools->Display)
{
    char sLabel[ MDisplay::MaxString ] = { 0 };
    strcpy( sLabel, "  OLMORO  *  BALSAT  " );
    Display->getTextLabel( sLabel );

    latrus = Tools->readNvsBool( MNvs::nQulon, MNvs::kQulonLocal, true );
    mode   = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, 0 );   // Индекс массива

    textMode( mode );
    Tools->powInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonPowInd, 3); // 3 - дефолтный индекс массива
    Tools->akbInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonAkbInd, 3); // Индекс массива с набором батарей
    Tools->setVoltageNom( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbU, Tools->akb[3][0]) );   // Начальный выбор 12 вольт
    Tools->setCapacity( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbAh, Tools->akb[3][1]) );   //             55 Ач

    Tools->postpone = Tools->readNvsInt( MNvs::nQulon, MNvs::kQulonPostpone,  3 );

    // Калибровки измерителей
    Board->voltageMultiplier  = Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonVmult,   1.00f ); 
    Board->voltageOffset      = Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonVoffset, 0.00f );
    Board->currentMultiplier  = Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonImult,   1.40f ); 
    Board->currentOffset      = Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonIoffset, 0.00f ); 
}

void MDispatcher::run()
{
    // Индикация при инициализации процедуры выбора режима работы
    Display->voltage( Board->getRealVoltage(), 2 );
    Display->current( Board->getRealCurrent(), 1 );

    // Выдерживается период запуска для вычисления амперчасов
    if (State)
    {
        // rabotaem so state mashinoj
        MState * newState = State->fsm();      
        if (newState != State)                  //state changed!
        {
            delete State;
            State = newState;
        } 
        //esli budet 0, na sledujushem cikle uvidim
    }
    else //state ne opredelen (0) - vybiraem ili pokazyvaem rezgim
    {
        if (Tools->Keyboard->getKey(MKeyboard::UP_CLICK))
        { 
            if (mode == (int)SERVICE) mode = OPTIONS;
            else mode++;
            textMode( mode );
        }

        if (Tools->Keyboard->getKey(MKeyboard::DN_CLICK))
        { 
            if (mode == (int)OPTIONS) mode = SERVICE;
            else mode--;
            textMode( mode );
        }

        if (Tools->Keyboard->getKey(MKeyboard::B_CLICK))
        {
            // Запомнить крайний выбор режима
            Tools->writeNvsInt( MNvs::nQulon, "mode", mode );
            //Tools->writeNvsInt( MNvs::nQulon, MNvs::kQulonMode );

            // Serial.print("Available heap: "); Serial.println(ESP.getFreeHeap());
            // Serial.print("Core ID: ");        Serial.println(xPortGetCoreID());

            switch (mode)
            {
                case OPTIONS:
                    State = new OptionFsm::MStart(Tools);
                break;

              #ifdef TEMPLATE_ENABLE
                case TEMPLATE:
                    State = new TemplateFsm::MStart(Tools);
                break;
              #endif

              #ifdef DC_SUPPLY_ENABLE
                case DCSUPPLY:
                    State = new DcSupplyFsm::MStart(Tools);
                break; 
              #endif

              #ifdef PULSE_GEN_ENABLE
                case PULSEGEN:
                    State = new PulseGenFsm::MStart(Tools);
                break;
              #endif

              #ifdef CCCV_CHARGE_ENABLE
                case CCCVCHARGE:
                    State = new CcCvFsm::MStart(Tools);
                break;
              #endif

              #ifdef PULSE_CHARGE_ENABLE           
                case PULSECHARGE:
                      State = new ExChargerFsm::MStart(Tools);
                break;
              #endif

              #ifdef RECOVERY_ENABLE
                case RECOVERY:
                    State = new RecoveryFsm::MStart(Tools);
                break;
              #endif

              #ifdef STORAGE_ENABLE
                case STORAGE:
                    State = new StorageFsm::MStart(Tools);
                break;
              #endif

              #ifdef DEVICE_ENABLE
                case DEVICE:
                    State = new DeviceFsm::MStart(Tools);
                break;
              #endif 

              case SERVICE:
                  State = new ServiceFsm::MStart(Tools);
              break;
              default:
              break;
            }
        } // !B_CLICK
    }
}

void MDispatcher::textMode(int mode)
{
    char sMode[ MDisplay::MaxString ] = { 0 };
    char sHelp[ MDisplay::MaxString ] = { 0 };

    switch(mode)
    {
        case OPTIONS:
            sprintf( sMode, "OPTIONS: BATT.SELECT," );
            sprintf( sHelp, "CALIBRATION,TIMER ETC" );
        break;

      #ifdef TEMPLATE_ENABLE
        case TEMPLATE:
            sprintf( sMode, "     TEMPLATE:        " );
            sprintf( sHelp, "EXAMPLE MODE FOR USER " );
        break;
      #endif

      #ifdef DC_SUPPLY_ENABLE
        case DCSUPPLY:
            sprintf( sMode, "     DC DCSUPPLY:    " );
            sprintf( sHelp, " SIMPLE POWER SUPPLY " );
        break;
      #endif

      #ifdef PULSE_GEN_ENABLE
        case PULSEGEN:
            sprintf( sMode, "    PULSE SUPPLY:    " );
            sprintf( sHelp, "   EXTENDED SUPPLY   " );
        break;
      #endif

      #ifdef CCCV_CHARGE_ENABLE
        case CCCVCHARGE:
            sprintf( sMode, "    CC/CV CHARGE:    " );
            sprintf( sHelp, " U/D-OTHER  B-SELECT " );
        break;
      #endif

      #ifdef PULSE_CHARGE_ENABLE
        case PULSECHARGE:
            sprintf( sMode, "    PULSE CHARGE:    " );
            sprintf( sHelp, "   EXTENDED CHARGE   " );
        break;
      #endif

      #ifdef RECOVERY_ENABLE
        case RECOVERY:
            sprintf( sMode, "  RECOVERY BATTERY:  " );
            sprintf( sHelp, "    DESULFATATION    " );
        break;
      #endif

      #ifdef STORAGE_ENABLE
        case STORAGE:
            sprintf( sMode, "   STORAGE BATTERY:  " );
            sprintf( sHelp, "  ENSURE A LONG LIFE " );
        break;
      #endif

      #ifdef DEVICE_ENABLE
        case DEVICE:
            sprintf( sMode, " INSTRUMENT SETTING: " );
            sprintf( sHelp, " THE DEVELOPER TOOLS " );
        break;
      #endif 

        case SERVICE:
            sprintf( sMode, "  BATTERY SERVICE:   " );
            sprintf( sHelp, " ADJUSTING THE DEVICE" );
        break;

      default:
          sprintf( sMode, "  ERROR:             ");
          sprintf( sHelp, "  UNIDENTIFIED MODE  " );
      break;
    }
  
    Display->getTextMode( sMode );
    Display->getTextHelp( sHelp );
  //  Serial.println( sMode);
}
