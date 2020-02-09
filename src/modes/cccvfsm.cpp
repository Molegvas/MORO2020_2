/*
    cccvfsm.cpp
    Конечный автомат заряда в режиме CC/CV.
    Такой алгоритм обеспечивает достаточно быстрый и «бережный» режим заряда аккумулятора. Для исключения
    долговременного пребывания аккумулятора в конце процесса заряда устройство переходит в режим поддержания 
    (компенсации тока саморазряда) напряжения на аккумуляторе. Такой алгоритм называется трехступенчатым. 
    График такого алгоритма заряда представлен на рисунке  http://www.balsat.ru/statia2.php
    27.05.2019 
*/

#include "modes/cccvfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "measure/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>

#include <string>


namespace CcCvFsm
{
    // Состояние "Старт", инициализация выбранного режима работы (Заряд CCCV).
    MStart::MStart(MTools * Tools) : MState(Tools)
    {
        // Параметры заряда из энергонезависимой памяти, Занесенные в нее при предыдущих включениях, как и
        // выбранные ранее номинальные параметры батареи (напряжение, емкость).
        // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
        Tools->setVoltageMax( Tools->readNvsFloat("cccv", "voltMax", MChConsts::voltageMaxFactor * Tools->getVoltageNom()) );
        Tools->setVoltageMin( Tools->readNvsFloat("cccv", "voltMin", MChConsts::voltageMinFactor * Tools->getVoltageNom()) );
        Tools->setCurrentMax( Tools->readNvsFloat("cccv", "currMax", MChConsts::currentMaxFactor * Tools->getCapacity()) );
        Tools->setCurrentMin( Tools->readNvsFloat("cccv", "currMin", MChConsts::currentMinFactor * Tools->getCapacity()) );

        // Индикация
        #ifdef OLED_1_3
            // Oled->showLine3Akb( Tools->getVoltageNom(), Tools->getCapacity() );              // example: "  12В  55Ач  "
            // Oled->showLine1Time(0);                         // уточнить
            // Oled->showLine1Ah(0.0);                         // уточнить
        #endif
        Display->getTextMode( (char*) "   CC/CV SELECTED    " );
        Display->getTextHelp( (char*) "  P-DEFINE  C-START  " );
        Display->progessBarOff();
//Display->fulfill( 0 );                                // GRAY
    }
    MState * MStart::fsm()
    {
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 1 );

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
                return new MPostpone(Tools);
            case MKeyboard::P_CLICK :
                return new MSetCurrentMax(Tools);      // Выбрано уточнение настроек заряда.
            default:;
        }

        return this;
    };

    // // Выбор заводских параметров или пользовательских ("профиль")
    // MSetFactory::MSetFactory(MTools * Tools) : MState(Tools)
    // {
    //     // Индикация
    //     #ifdef OLED_1_3
    //         // Oled->showLine4Text("   Factory   ");
    //         // Oled->showLine3Text("     Y/NO    ");
    //         // Oled->showLine2Text("  B-yes,  C-no  ");
    //     #endif

    //     #ifdef TFT_1_44
    //         // no leds
    //     #endif
    //     #ifdef TFT_1_8
    //         // no leds
    //     #endif
    // }
    // MState * MSetFactory::fsm()
    // {
    //     switch ( Keyboard->getKey() )
    //     {
    //         case MKeyboard::C_CLICK :
    //             return new MSetCurrentMax(Tools);           // Отказ в восстановлении заводских параметров.
    //         case MKeyboard::B_CLICK :
    //             Tools->clearAllKeys("cccv");                // Очистка в энергонезависимой памяти.

    //             // Восстановление параметров заряда (после очистки – дефолтными значениями).
    //             Tools->setVoltageMax( Tools->readNvsFloat("cccv", "voltMax", MChConsts::voltageMaxFactor * Tools->getVoltageNom()) );
    //             Tools->setVoltageMin( Tools->readNvsFloat("cccv", "voltMin", MChConsts::voltageMinFactor * Tools->getVoltageNom()) );
    //             Tools->setCurrentMax( Tools->readNvsFloat("cccv", "currMax", MChConsts::currentMaxFactor * Tools->getCapacity()) );
    //             Tools->setCurrentMin( Tools->readNvsFloat("cccv", "currMin", MChConsts::currentMinFactor * Tools->getCapacity()) );
    //             return new MSetCurrentMax(Tools);
    //         default:;
    //     }

    //     return this;
    // };

    // Коррекция максимального тока заряда.
    MSetCurrentMax::MSetCurrentMax(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) "U/D-SET CURRENT MAX" );
        Display->getTextHelp( (char*) "  B-SAVE  C-START  " );
    }
    MState * MSetCurrentMax::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_LONG_CLICK :               // Отказ от продолжения ввода параметров - стоп
                return new MStop(Tools);
            case MKeyboard::C_CLICK :                    // Отказ от дальнейшего ввода параметров - исполнение
                return new MPostpone(Tools);
            case MKeyboard::B_CLICK :                    // Сохранить и перейти к следующему параметру
                Tools->saveFloat( "cccv", "currMax", Tools->getCurrentMax() ); 
                return new MSetVoltageMax(Tools);

            case MKeyboard::UP_CLICK :
                Tools->incCurrentMax( 0.1f, false );     // По кольцу? - Нет
                break;
            case MKeyboard::DN_CLICK:
                Tools->decCurrentMax( 0.1f, false );
                break;
            case MKeyboard::UP_AUTO_CLICK:
                Tools->incCurrentMax( 0.1f, false );
                break;
            case MKeyboard::DN_AUTO_CLICK:
                Tools->decCurrentMax( 0.1f, false );
                break;
            default:;
        }
        // Индикация ввода
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Tools->getCurrentMax(), 1 );

        return this;
    };

    // Коррекция максимального напряжения.
    MSetVoltageMax::MSetVoltageMax(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) "U/D-SET VOLTAGE MAX" );
        Display->getTextHelp( (char*) "  B-SAVE  C-START  " );
    }
    MState * MSetVoltageMax::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_LONG_CLICK :               // Отказ от продолжения ввода параметров - стоп
                return new MStop(Tools);
            case MKeyboard::C_CLICK :                    // Отказ от дальнейшего ввода параметров - исполнение
                return new MPostpone(Tools);
            case MKeyboard::B_CLICK :                    // Сохранить и перейти к следующему параметру
                Tools->saveFloat( "cccv", "voltMax", Tools->getVoltageMax() ); 
                return new MSetCurrentMin(Tools);

            case MKeyboard::UP_CLICK :
                Tools->incVoltageMax( 0.1f, false );     // По кольцу? - Нет
                break;
            case MKeyboard::DN_CLICK:
                Tools->decVoltageMax( 0.1f, false );
                break;
            case MKeyboard::UP_AUTO_CLICK:
                Tools->incVoltageMax( 0.1f, false );
                break;
            case MKeyboard::DN_AUTO_CLICK:
                Tools->decVoltageMax( 0.1f, false );
                break;
            default:;
        }
        // Индикация ввода
        Display->voltage( Tools->getVoltageMax(), 1 );
        Display->current( Board->getRealCurrent(), 1 );

        return this;
    };

    // Коррекция минимального тока заряда
    MSetCurrentMin::MSetCurrentMin(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) "U/D-SET CURRENT MIN" );
        Display->getTextHelp( (char*) "  B-SAVE  C-START  " );
    }   
    MState * MSetCurrentMin::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_LONG_CLICK :               // Отказ от продолжения ввода параметров - стоп
                return new MStop(Tools);
            case MKeyboard::C_CLICK :                    // Отказ от дальнейшего ввода параметров - исполнение
                return new MPostpone(Tools);
            case MKeyboard::B_CLICK :                    // Сохранить и перейти к следующему параметру
                Tools->saveFloat( "cccv", "currMin", Tools->getCurrentMin() ); 
                return new MSetVoltageMin(Tools);

            case MKeyboard::UP_CLICK :
                Tools->incCurrentMin( 0.1f, false );     // По кольцу? - Нет
                break;
            case MKeyboard::DN_CLICK:
                Tools->decCurrentMin( 0.1f, false );
                break;
            case MKeyboard::UP_AUTO_CLICK:
                Tools->incCurrentMin( 0.1f, false );
                break;
            case MKeyboard::DN_AUTO_CLICK:
                Tools->decCurrentMin( 0.1f, false );
                break;
            default:;
        }
        // Индикация ввода
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Tools->getCurrentMin(), 1 );

        return this;
    };

    // Коррекция минимального напряжения окончания заряда.
    MSetVoltageMin::MSetVoltageMin(MTools * Tools) : MState(Tools)
    {
    // Индикация помощи
    Display->getTextMode( (char*) "U/D-SET VOLTAGE MIN" );
    Display->getTextHelp( (char*) "  B-SAVE  C-START  " );
    }   
    MState * MSetVoltageMin::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_LONG_CLICK :               // Отказ от продолжения ввода параметров - стоп
                return new MStop(Tools);
            case MKeyboard::C_CLICK :                    // Отказ от дальнейшего ввода параметров - исполнение
                return new MPostpone(Tools);
            case MKeyboard::B_CLICK :                    // Сохранить и перейти к следующему параметру
                Tools->saveFloat( "cccv", "voltMin", Tools->getVoltageMin() ); 
                return new MPostpone(Tools);

            case MKeyboard::UP_CLICK :
                Tools->incVoltageMin( 0.1f, false );     // По кольцу? - Нет
                break;
            case MKeyboard::DN_CLICK:
                Tools->decVoltageMin( 0.1f, false );
                break;
            case MKeyboard::UP_AUTO_CLICK:
                Tools->incVoltageMin( 0.1f, false );
                break;
            case MKeyboard::DN_AUTO_CLICK:
                Tools->decVoltageMin( 0.1f, false );
                break;
            default:;
        }
        // Индикация ввода
        Display->voltage( Tools->getVoltageMin(), 1 );
        Display->current( Board->getRealCurrent(), 1 );

        return this;
    };
// ************************************************************************************** OLD

    // Задержка включения (отложенный старт).
    // Время ожидания старта задается в настройках.
    MPostpone::MPostpone(MTools * Tools) : MState(Tools)
    {
        // Параметр задержки начала заряда из энергонезависимой памяти, при первом включении - заводское
        Tools->postpone = Tools->readNvsInt( "qulon", "postp", 0 );
                
        // Индикация помощи
        Display->getTextMode( (char*) " C-START CLONG-STOP" );
        Display->getTextHelp( (char*) " BEFORE THE START: " );

        // Инициализация счетчика времени до старта
        Tools->setTimeCounter( Tools->postpone * 36000 );                // Отложенный старт ( * 0.1s )
    }     
    MState * MPostpone::fsm()
    {
        if( Tools->postponeCalculation() ) return new MUpCurrent(Tools);    // Старт по времени

        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_LONG_CLICK :        // Досрочное прекращение заряда оператором
                return new MStop(Tools);
            case MKeyboard::C_CLICK :
                return new MUpCurrent(Tools);
            default:;
        }
        // Индикация в период ожидания старта (обратный отсчет)

        // ...

        return this;
    };

// **************************************************************************************

    // Начальный этап заряда - ток поднимается не выше заданного уровня,
    // при достижении заданного максимального напряжения переход к его удержанию.
    // Здесь и далее подсчитывается время и отданный заряд, а также
    // сохраняется возможность прекращения заряда оператором.
    MUpCurrent::MUpCurrent(MTools * Tools) : MState(Tools)
    {   
        // Индикация помощи
        Display->getTextMode( (char*) " UP CURRENT TO MAX " );
        Display->getTextHelp( (char*) "       C-STOP      " );

        // Обнуляются счетчики времени и отданного заряда
        Tools->clrTimeCounter();
        Tools->clrAhCharge();

        // Включение преобразователя и коммутатора (уточнить порядок?)
        Board->setDischargeAmp( 0.0f );
        Board->setCurrentAmp( 0.0f );               // Ток в начале будет ограничен
        Board->powOn();     Board->swOn();          // Включение преобразователя и коммутатора.
   
        // Настройка ПИД-регулятора
        Tools->initPid( MPidConstants::outputMin,
                        MPidConstants::outputMaxFactor * Tools->getCurrentMax(),
                        MPidConstants::k_p,
                        MPidConstants::k_i,
                        MPidConstants::k_d,
                        MPidConstants::bangMin,
                        MPidConstants::bangMax,
                        MPidConstants::timeStep );
        Tools->setSetPoint( Tools->getVoltageMax() );                                      //лишнее?

        // ???
        Board->setVoltageVolt( Tools->getVoltageMax() * 1.05f );     // Voltage limit
    }     
    MUpCurrent::MState * MUpCurrent::fsm()
    {
        Tools->chargeCalculations();                                                    // Подсчет отданных ампер-часов.

        // После пуска короткое нажатие кнопки "C" производит отключение тока.
        if(Keyboard->getKey(MKeyboard::C_CLICK)) { return new MStop(Tools); }    

        // Проверка напряжения и переход на поддержание напряжения.
        if( Board->getRealVoltage() >= Tools->getVoltageMax() ) { return new MKeepVmax(Tools); }

        // Коррекция источника тока по измерению (установщик откалиброван с некоторым завышением)
        if( Board->getRealCurrent() > Tools->getCurrentMax() ) { Tools->adjustIntegral( -0.250f ); } // -0.025A

        Tools->runPid( Board->getRealVoltage() );            // Подъём и поддержание тока.
        
        // Индикация фазы подъема тока не выше заданного
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 1 );

        Display->progessBarExe( MDisplay::GREEN );
        Display->duration( Tools->getChargeTimeCounter() );
        Display->amphours( Tools->getAhCharge() );
        
        return this;
    };

    // Вторая фаза заряда - достигнуто заданное максимальное напряжение.
    // При падении тока ниже заданного уровня - переход к третьей фазе.
    MKeepVmax::MKeepVmax(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) "KEEP VOLTAGE TO MAX" );
        Display->getTextHelp( (char*) "       C-STOP      " );

        // Порог регулирования по напряжению
        Tools->setSetPoint( Tools->getVoltageMax() );
    }       
    MState * MKeepVmax::fsm()
    {
        Tools->chargeCalculations();                                                    // Подсчет отданных ампер-часов.

        if (Keyboard->getKey(MKeyboard::C_CLICK)) { return new MStop(Tools); }                // Окончание процесса оператором.

        if( Board->getRealCurrent() <= Tools->getCurrentMin() ) { return new MKeepVmin(Tools); } // Ожидание спада тока ниже C/20 ампер.

        // Коррекция
        if( Board->getRealCurrent() > Tools->getCurrentMax() ) { Tools->adjustIntegral( -0.250f ); } // -0.025A

        Tools->runPid( Board->getRealVoltage() ); 
        
                                                  // Поддержание максимального напряжения.
        // Индикация фазы удержания максимального напряжения
        // Реальные ток и напряжения - без изменения, можно не задавать?
        
        Display->progessBarExe( MDisplay::YELLOW );
        Display->duration( Tools->getChargeTimeCounter() );
        Display->amphours( Tools->getAhCharge() );

        return this;
    };

    // Третья фаза заряда - достигнуто снижение тока заряда ниже заданного предела.
    // Проверки различных причин завершения заряда.
    MKeepVmin::MKeepVmin(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) " KEEP VOLTAGE MIN  " );
        Display->getTextHelp( (char*) "       C-STOP      " );

        // Порог регулирования по напряжению
        Tools->setSetPoint( Tools->getVoltageMin() );
                
    }     
    MState * MKeepVmin::fsm()
    {
        Tools->chargeCalculations();                                                    // Подсчет отданных ампер-часов.

        if (Keyboard->getKey(MKeyboard::C_CLICK)) { return new MStop(Tools); }                // Окончание процесса оператором.

        // Здесь возможны проверки других условий окончания заряда
        // if( ( ... >= ... ) && ( ... <= ... ) )  { return new MStop(Tools); }

        // Максимальное время заряда, задается в "Настройках"
        if( Tools->getChargeTimeCounter() >= ( Tools->charge_time_out_limit * 36000 ) ) { return new MStop(Tools); }

        // Необходимая коррекция против выброса тока
        if( Board->getRealCurrent() > Tools->getCurrentMax() ) { Tools->adjustIntegral( -0.250f ); }        // -0.025A
        Tools->runPid( Board->getRealVoltage() );           // Регулировка по напряжению

        Display->progessBarExe( MDisplay::MAGENTA );
        Display->duration( Tools->getChargeTimeCounter() );
        Display->amphours( Tools->getAhCharge() );

        return this;
    };

    // Процесс выхода из режима заряда - до нажатия кнопки "С" удерживается индикация о продолжительности и отданном заряде.
    MStop::MStop(MTools * Tools) : MState(Tools)
    {
        Tools->shutdownCharge();
        Display->getTextMode( (char*) "   CC/CV CHARGE OFF  " );
        Display->getTextHelp( (char*) "  P-FACTORY   C-EXIT " );
        Display->progessBarStop();

    }    
    MState * MStop::fsm()
    {
        
        if(Keyboard->getKey(MKeyboard::C_CLICK))
        { 
            //Tools->activateExit("  CC/CV  заряд  ");
            Display->getTextMode( (char*) "    CC/CV CHARGE     " );
            Display->getTextHelp( (char*) " U/D-OTHER  B-SELECT " );
            Display->progessBarOff();

            return nullptr;                             // Возврат к выбору режима
        }
        return this;
    };
};
