/*
 * storagefsm.cpp
 * В прототипе - опция расширенного заряда, в этой версии STORAGE, Storage
 * На дисплее при выборе режима отображается как "    Хранение    "
 * Проект: Q921-502 06.06.2019         !!! Тестовый разряд - эксперимент по определению эффективности теплоотвода !!!
 * 
*/

#include "modes/storagefsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/moverseer.h"
#include "display/mdisplay.h"
#include "measure/mkeyboard.h"
#include <Arduino.h>

namespace StorageFsm
{
    // Состояние "Старт", инициализация выбранного режима работы
    MStart::MStart(MTools * Tools) : MState(Tools)
    {
        // Индикация
        Display->getTextMode( (char*) " DISCHARGE SELECTED  " );
        Display->getTextHelp( (char*) "  P-DEFINE  C-START  " );
        Display->progessBarOff();

        // Параметры заряда из энергонезависимой памяти, при первом включении - заводские
        Tools->setVoltageMin( Tools->readNvsFloat("storage", "voltMin", 11.8f) );
        Tools->setCurrentDis( Tools->readNvsFloat("storage", "currDis",  1.0f) );

    }
    // Старт тестового разряда
    MState * MStart::fsm()
    {
        if( Keyboard->getKey(MKeyboard::C_CLICK))
        {
            // Старт без уточнения параметров (быстрый старт),
            // максимальный ток и напряжение окончания - паспортные, исходя из параметров АКБ
            // Восстановленные на предыдущем шаге данные заменяются на паспортные
            Tools->setVoltageMin( Tools->getVoltageNom() * 0.96f );
            Tools->setCurrentMax( Tools->getCapacity() / 10.0f );

            return new MExecution(Tools);
        }

        if( Keyboard->getKey(MKeyboard::P_CLICK)) return new MSetCurrentDis(Tools);
        return this;                                    // Состояние не меняется
    };

    // Выбор максимального тока разряда
    MSetCurrentDis::MSetCurrentDis(MTools * Tools) : MState(Tools)
    {
                    // Индикация
//            Oled->showLine4Text("  Imax разр. ");
//            Oled->showLine3MaxI( Tools->getCurrentDis() );
            Tools->showUpDn(); // " UP/DN, В-выбор "

    }
    MState * MSetCurrentDis::fsm()
    {
        // Выход без сохранения корректируемого параметра
        if( Keyboard->getKey(MKeyboard::C_LONG_CLICK)) { Tools->shutdownCharge(); return new MStop(Tools); }

        if( Keyboard->getKey(MKeyboard::UP_CLICK))      { Tools->incCurrentDis( 0.05, false ); return this; }     // Добавить 100 мА
        if( Keyboard->getKey(MKeyboard::DN_CLICK))      { Tools->decCurrentDis( 0.05f, false ); return this; }
        //if( Keyboard->getKey(MKeyboard::UP_LONG_CLICK)) { Tools->incCurrentDis( 0.25f, false ); return this; }     // Добавить 500 мА
        //if( Keyboard->getKey(MKeyboard::DN_LONG_CLICK)) { Tools->decCurrentDis( 0.25f, false ); return this; }

        if( Keyboard->getKey(MKeyboard::B_CLICK))
        {
            Tools->writeNvsFloat( "storage", "currDis", Tools->getCurrentDis() );
            return new MSetVoltageMin(Tools);
        }
        return this;
    };

    // Выбор напряжения окончания разряда
    MSetVoltageMin::MSetVoltageMin(MTools * Tools) : MState(Tools)
    {
                    // Индикация
//            Oled->showLine4Text("  Vmin разр. ");
//            Oled->showLine3MaxU( Tools->getVoltageMin() );
            Tools->showUpDn(); // " UP/DN, В-выбор "

    }
    MState * MSetVoltageMin::fsm()
    {
        // Выход без сохранения корректируемого параметра
        if( Keyboard->getKey(MKeyboard::C_LONG_CLICK)) { Tools->shutdownCharge(); return new MStop(Tools); }

        if( Keyboard->getKey(MKeyboard::UP_CLICK))      { Tools->incVoltageMin( 0.1f, false ); return this; }
        if( Keyboard->getKey(MKeyboard::DN_CLICK))      { Tools->decVoltageMin( 0.1f, false ); return this; }
        if( Keyboard->getKey(MKeyboard::UP_LONG_CLICK)) { Tools->incVoltageMin( 1.0f, false ); return this; }
        if( Keyboard->getKey(MKeyboard::DN_LONG_CLICK)) { Tools->decVoltageMin( 1.0f, false ); return this; }

        if( Keyboard->getKey(MKeyboard::B_CLICK))
        {   // Выбор заносится в энергонезависимую память
            Tools->writeNvsFloat( "storage", "voltMin", Tools->getVoltageMin() );
            return new MExecution(Tools);
        }
        return this;
    };

    MExecution::MExecution(MTools * Tools) : MState(Tools)
    {
                    //Tools->clrAhCharge();                  // Обнуляются счетчики времени и отданного заряда

            // Задаются начальные напряжение и ток
            Board->setVoltageVolt( 0.0f );        //Tools->getVoltageMax() );         // Voltage limit
            Board->setCurrentAmp( 0.0f );
            Board->setDischargeAmp( Tools->getCurrentDis() );  // 
            Board->powOn();   Board->swOn();                    // Включение преобразователя и коммутатора. 
            #ifdef V22
                Board->ledsGreen();                  // Зеленый светодиод - процесс разряда запущен
            #endif
            // Задание отображения на экране дисплея построчно (4-я строка - верхняя)
            // Oled->showLine4RealVoltage();
            // Oled->showLine3RealCurrent();
            // Oled->showLine2Text(" Идёт разряд... ");            // 
            // Oled->showLine1Time( Tools->getChargeTimeCounter() );
            // //Oled->showLine1Ah( Tools->getAhCharge() );
            // Oled->showLine1Celsius( Board->Overseer->getCelsius() );


    //      Tools->setSetPoint( Tools->getVoltageMax() );

        //   #ifdef DEBUG_SUPPLY
        //       Serial.println(" Источник питания стартовал с параметрами:");
        //       Serial.print("  Voltage max, B  *** = ");  Serial.println( Tools->getVoltageMax() );
        //       Serial.print("  Current max, A  *** = ");  Serial.println( Tools->getCurrentMax() );
        //   #endif

//                                                            // задающей напряжение цепи


    }
    MState * MExecution::fsm()
    {
        Tools->chargeCalculations();

        if(Keyboard->getKey(MKeyboard::C_CLICK)) { return new MStop(Tools); } 

        // Проверка напряжения и переход на поддержание напряжения.
        if( Board->getRealVoltage() <= Tools->getVoltageMin() ) { return new MStop(Tools); }

        // Do nothing
        return this;
    };

    // // Процесс выхода из тестового режима - до нажатия кнопки "С" удерживается индикация
    // // о продолжительности и отданном заряде.
    // MState * MStop::fsm()
    // {
    //     if(Keyboard->getKey(MKeyboard::C_CLICK)) 
    //     {
    //         Tools->activateExit("    Хранение    ");
    //         // #ifdef DEBUG_SUPPLY
    //         //     Serial.println("DcSupply: Exit"); 
    //         //     Serial.print("\t charge = ");   Serial.println( Tools->getAhCharge() );
    //         // #endif  
    //         return 0;   // Возврат к выбору режима
    //     }
    //     return this;
    // };

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
