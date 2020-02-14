
/*
    exchargerfsm.cpp
    Конечный автомат заряда в режиме    "Импульсный заряд"
    (в разработке)
*/

#include "modes/exchargerfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "measure/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>


namespace ExChargerFsm
{
    // Состояние "Старт", инициализация выбранного режима работы
    MStart::MStart(MTools * Tools) : MState(Tools)
    {
        // Индикация
        Display->getTextMode( (char*) "  PULSE CH.  C-EXIT  " );
        Display->getTextHelp( (char*) "  IN THE DEVELOPMENT " );
        Display->progessBarOff();
    
        // Параметры заряда из энергонезависимой памяти, Занесенные в нее при предыдущих включениях, как и
        // выбранные ранее номинальные параметры батареи (напряжение, емкость).
        // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
        Tools->setVoltageMax( Tools->readNvsFloat("e-charge", "voltMax", MChConsts::voltageMaxFactor * Tools->getVoltageNom()) );
        Tools->setVoltageMin( Tools->readNvsFloat("e-charge", "voltMin", MChConsts::voltageMinFactor * Tools->getVoltageNom()) );
        Tools->setCurrentMax( Tools->readNvsFloat("e-charge", "currMax", MChConsts::currentMaxFactor * Tools->getCapacity()) );
        Tools->setCurrentMin( Tools->readNvsFloat("e-charge", "currMin", MChConsts::currentMinFactor * Tools->getCapacity()) );
    }
    MState * MStart::fsm()
    {

        switch ( Keyboard->getKey() )    //Здесь так можно
        {
            case MKeyboard::C_CLICK :
                // Старт без уточнения параметров (здесь – для батарей типа AGM), 
                // максимальный ток и напряжение окончания - паспортные, исходя из параметров АКБ 
                // Выбор АКБ производится в "Настройках".
                        // MChConsts::xxxFactor * Tools->getXxxx(),

                Tools->setVoltageMax( MChConsts::voltageMaxFactor * Tools->getVoltageNom() );    // Например, voltageMax = 14.8;
                Tools->setVoltageMin( MChConsts::voltageMinFactor * Tools->getVoltageNom() );
                Tools->setCurrentMax( MChConsts::currentMaxFactor * Tools->getCapacity() );
                Tools->setCurrentMin( MChConsts::currentMinFactor * Tools->getCapacity() );
                return new MStop(Tools);     // MPostpone(Tools);
            case MKeyboard::P_CLICK :
                return new MStop(Tools);     //MSetCurrentMax(Tools);      // Выбрано уточнение настроек заряда.
            default:;
        }

        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 1 );
        return this;
    };


    // ...


    // Завершение режима заряда - до нажатия кнопки "С" удерживается индикация 
    // о продолжительности и отданном заряде.
    MStop::MStop(MTools * Tools) : MState(Tools)
    {

        Tools->shutdownCharge();
        Display->getTextMode( (char*) "   PULSE CHARGE END  " );
        Display->getTextHelp( (char*) "              C-EXIT " );
        Display->progessBarStop();
    }    
    MState * MStop::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                return new MExit(Tools);
            default:;
        
//Display->progessBarOff();
        }
        return this;
    };

    // Процесс выхода из режима заряда - до нажатия кнопки "С"
    // удерживается индикация о продолжительности и отданном заряде.
    MExit::MExit(MTools * Tools) : MState(Tools)
    {
        Tools->shutdownCharge();
        Display->getTextMode( (char*) "   PULSE CHARGE OFF  " );
        Display->getTextHelp( (char*) "              C-EXIT " );
        Display->progessBarOff();
    }    
    MState * MExit::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                // Надо бы восстанавливать средствами диспетчера...
                Display->getTextMode( (char*) "     PULSE CHARGE:   " );
                Display->getTextHelp( (char*) "   EXTENDED CHARGE   " );
                return nullptr;                             // Возврат к выбору режима
            default:;
        }
        return this;
    };
};
