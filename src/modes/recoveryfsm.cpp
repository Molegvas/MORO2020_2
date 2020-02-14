/*
 * recoveryfsm.cpp - стадия экспериментов
 * В прототипе - опция расширенного заряда, в этой версии RECOVER, Recover
 * На дисплее при выборе режима отображается как " Восстановление "
 * Проект: Q921-Andr 02.01.2019         !!! Местами настройки только для 12-вольтовой батареи !!!
 * Автор: Олег Морозов 
 * Консультант: Андрей Морозов
*/

#include "modes/recoveryfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "measure/mkeyboard.h"
#include "display/mdisplay.h"
#include "mstate.h"
#include <Arduino.h>

namespace RecoveryFsm
{
    // Состояние "Старт", инициализация выбранного режима работы
    MStart::MStart(MTools * Tools) : MState(Tools)
    {
        // Индикация
        Display->getTextMode( (char*) " RECOVERY CH. C-EXIT " );
        Display->getTextHelp( (char*) "  IN THE DEVELOPMENT " );
        Display->progessBarOff();
    
        // Параметры заряда из энергонезависимой памяти, Занесенные в нее при предыдущих включениях, как и
        // выбранные ранее номинальные параметры батареи (напряжение, емкость).
        // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
    //     Tools->setVoltageMax( Tools->readNvsFloat("recovery", "voltMax", MChConsts::voltageMaxFactor * Tools->getVoltageNom()) );
    //     Tools->setVoltageMin( Tools->readNvsFloat("recovery", "voltMin", MChConsts::voltageMinFactor * Tools->getVoltageNom()) );
    //     Tools->setCurrentMax( Tools->readNvsFloat("recovery", "currMax", MChConsts::currentMaxFactor * Tools->getCapacity()) );
    //     Tools->setCurrentMin( Tools->readNvsFloat("recovery", "currMin", MChConsts::currentMinFactor * Tools->getCapacity()) );
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

                // Tools->setVoltageMax( MChConsts::voltageMaxFactor * Tools->getVoltageNom() );    // Например, voltageMax = 14.8;
                // Tools->setVoltageMin( MChConsts::voltageMinFactor * Tools->getVoltageNom() );
                // Tools->setCurrentMax( MChConsts::currentMaxFactor * Tools->getCapacity() );
                // Tools->setCurrentMin( MChConsts::currentMinFactor * Tools->getCapacity() );
                return new MStop(Tools);     // MPostpone(Tools);
            case MKeyboard::P_CLICK :
                return new MStop(Tools);     //MSetCurrentMax(Tools);      // Выбрано уточнение настроек заряда.
            default:;
        }

        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 1 );
        return this;
    };

    // ... другие состояния

    // Завершение режима заряда - до нажатия кнопки "С" удерживается индикация 
    // о продолжительности и отданном заряде.
    MStop::MStop(MTools * Tools) : MState(Tools)
    {

        Tools->shutdownCharge();
        Display->getTextMode( (char*) "     RECOVERY END    " );
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

    // Процесс выхода из режима восстановления - до нажатия кнопки "С"
    // удерживается индикация о продолжительности и заряде.
    MExit::MExit(MTools * Tools) : MState(Tools)
    {
        Tools->shutdownCharge();
        Display->getTextMode( (char*) "     RECOVERY OFF    " );
        Display->getTextHelp( (char*) "              C-EXIT " );
        Display->progessBarOff();
    }    
    MState * MExit::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                // Надо бы восстанавливать средствами диспетчера...
                Display->getTextMode( (char*) "       RECOVERY:     " );
                Display->getTextHelp( (char*) "   RECOVERY CHARGE   " );
                return nullptr;                             // Возврат к выбору режима
            default:;
        }
        return this;
    };


};
