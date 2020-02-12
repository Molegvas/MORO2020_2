/*


*/






#include "modes/optionsfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "measure/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>


namespace OptionFsm
{
    // Переменные
    //float vOffset   = 0.0f;
    
    // Состояние "Старт", инициализация выбранного режима работы.
    MStart::MStart(MTools * Tools) : MState(Tools)
    {
        #ifdef DEBUG_OPTIONS
            Serial.println("Options: Start");
        #endif
        // Индикация
        Display->getTextMode( (char*) "  OPTIONS SELECTED   " );
        Display->getTextHelp( (char*) "  P-DEFINE   C-EXIT  " );
    }
    MState * MStart::fsm()
    {
        switch ( Keyboard->getKey() )
        {
        case MKeyboard::C_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :
            // Продолжение выбора объекта настройки
            return new MSetPostpone(Tools);
        case MKeyboard::B_CLICK :
            return new MSetPostpone(Tools);     // MSelectBattery
        default :;
        }
        return this;
    };

//     MSelectBattery::MSelectBattery(MTools * Tools) : MState(Tools)
//     {
//         // Индикация
//         #ifdef OLED_1_3
// //            Oled->showLine4Text("   Батарея   ");
// //            Oled->showLine3Akb( Tools->getVoltageNom(), Tools->getCapacity() );          // example: "  12В  55Ач  "
//             Tools->showUpDn();                  // " UP/DN, В-выбор "
//         #endif
//     }
//     MState * MSelectBattery::fsm()
//     {
// //         if ( Keyboard->getKey(MKeyboard::C_CLICK)) { return new MExit(Tools); }    

// //         if( Keyboard->getKey(MKeyboard::UP_CLICK)) { Tools->incBattery(); return this; }
// //         if( Keyboard->getKey(MKeyboard::DN_CLICK)) { Tools->decBattery(); return this; } 

// //         if( Keyboard->getKey(MKeyboard::B_CLICK))              // Завершить выбор батареи
// //         {
// // //            Tools->saveBattery( "qulon" );           // Уточнить: общий для всех режимов?
// //         Tools->writeNvsInt( "qulon", "akbInd", Tools->getAkbInd() );

// //             return new MSetPostpone(Tools);
// //         }

//         switch ( Keyboard->getKey() )
//         {
//         case MKeyboard::C_LONG_CLICK :
//             return new MExit(Tools);
//         case MKeyboard::UP_CLICK :
//             Tools->incBattery();
//             break;  //    return this; 
//         case MKeyboard::DN_CLICK :
//             Tools->decBattery();
//             break;      // return this; } 
//         case MKeyboard::P_CLICK :
//             // Продолжение выбора объекта настройки
//             return new MSetPostpone(Tools);

//         case MKeyboard::B_CLICK :
//             // Завершить выбор батареи
//             Tools->writeNvsInt( "qulon", "akbInd", Tools->getAkbInd() );
//             return new MSetPostpone(Tools);
//         default :;
//         }
//         return this;
//     };

    MSetPostpone::MSetPostpone(MTools * Tools) : MState(Tools) 
    {
        // Подсказка
        Display->getTextMode( (char*) "  U/D-SET POSTPONE   " );
        Display->getTextHelp( (char*) "  P-DEFINE   C-EXIT  " );
    }
    MState * MSetPostpone::fsm()
    {
        switch ( Keyboard->getKey() )
        {
        case MKeyboard::C_LONG_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :
            return new MSetCurrentOffset(Tools);
        case MKeyboard::UP_CLICK :
        case MKeyboard::UP_AUTO_CLICK :
            Tools->postpone = Tools->upiVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
            break;
        case MKeyboard::DN_CLICK :
        case MKeyboard::DN_AUTO_CLICK :
            Tools->postpone = Tools->dniVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
            break;
        case MKeyboard::B_CLICK :
            Tools->saveInt( "qulon", "postp", Tools->postpone );   // Выбор заносится в энергонезависимую память
            // #ifdef DEBUG_OPTIONS
            //     Serial.println(Tools->postpone);
            // #endif
            return new MSetCurrentOffset(Tools);
        default :;
        }

        Display->duration( Tools->postpone, MDisplay::HOUR );

        return this;
    };

    MSetCurrentOffset::MSetCurrentOffset( MTools * Tools ) : MState(Tools) 
    {
        // Смещение восстановлено из nvs при инициализации прибора
        // Индикация помощи
        Display->getTextMode( (char*) " SET CURRENT OFFSET  " );
        Display->getTextHelp( (char*) " B-YES  P-NO  C-EXIT " );
        }
    MState * MSetCurrentOffset::fsm()
    {
        switch ( Keyboard->getKey() )
        {
        case MKeyboard::C_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :
            return new MSetVoltageOffset(Tools);
        case MKeyboard::UP_CLICK :
            //Tools->incCurrentOffset( 0.01f, false );
            Board->currentOffset = Tools->upfVal( Board->currentOffset, MOptConsts::c_offset_l, MOptConsts::c_offset_h, 0.01f );
            break;
        case MKeyboard::DN_CLICK :
            //Tools->decCurrentOffset( 0.01f, false );
            Board->currentOffset = Tools->dnfVal( Board->currentOffset, MOptConsts::c_offset_l, MOptConsts::c_offset_h, 0.01f );
            break;
        case MKeyboard::B_CLICK :
            Tools->saveInt( "qulon", "cOffset", Board->currentOffset );
            // #ifdef DEBUG_OPTIONS
            //     Serial.println(Board->currentOffset);
            // #endif
            return new MSetVoltageOffset(Tools);
        default :;
        }
        // Изменение смещения отображается на текущем значении 
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 2 );
        Display->duration( Tools->postpone, MDisplay::SEC );

        return this;
    };

    MSetVoltageOffset::MSetVoltageOffset( MTools * Tools ) : MState(Tools)
    {
        // Смещение восстановлено из nvs при инициализации прибора
        // Индикация помощи
        Display->getTextMode( (char*) " SET VOLTAGE OFFSET  " );
        Display->getTextHelp( (char*) " B-YES  P-NO  C-EXIT " );
    }
    MState * MSetVoltageOffset::fsm()
    {
        switch( Keyboard->getKey() )
        {
        case MKeyboard::C_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :
            return new MSetCcCvChargeFactory(Tools);
        case MKeyboard::UP_CLICK :
            //Tools->incVoltageOffset( 0.01f, false );
            Board->voltageOffset = Tools->upfVal( Board->voltageOffset, MOptConsts::v_offset_l, MOptConsts::v_offset_h, 0.01f );
            break;
        case MKeyboard::DN_CLICK :
            //Tools->decVoltageOffset( 0.01f, false );
            Board->voltageOffset = Tools->dnfVal( Board->voltageOffset, MOptConsts::v_offset_l, MOptConsts::v_offset_h, 0.01f );
            break;
        case MKeyboard::B_CLICK :
            Tools->saveInt( "qulon", "vOffset", Board->voltageOffset );
            // #ifdef DEBUG_OPTIONS
            //     Serial.println(Board->voltageOffset);
            // #endif
            return new MExit(Tools);
        default :;
        }
        // Изменение смещения отображается на текущем значении 
        Display->voltage( Board->getRealVoltage(), 2 );
        Display->current( Board->getRealCurrent(), 1 );
        return this;
    };





    MSetCcCvChargeFactory::MSetCcCvChargeFactory(MTools * Tools) : MState(Tools) 
    {
        // Индикация помощи
        Display->getTextMode( (char*) " SET CCCV CH.FACTORY " );
        Display->getTextHelp( (char*) " B-YES  P-NO  C-EXIT " );
    }
    MState * MSetCcCvChargeFactory::fsm()
    {
        switch ( Keyboard->getKey() )
        {
        case MKeyboard::C_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :
            return new MSetQulonFactory(Tools);
        case MKeyboard::B_CLICK :
            Tools->clearAllKeys("cccv");    // Выбор заносится в энергонезависимую память
            return new MSetQulonFactory(Tools);
        default :;
        }

        return this;
    };

    MSetQulonFactory::MSetQulonFactory(MTools * Tools) : MState(Tools) 
    {
        // Индикация помощи
        Display->getTextMode( (char*) "  SET QULON FACTORY  " );
        Display->getTextHelp( (char*) " B-YES  P-NO  C-EXIT " );
    }
    MState * MSetQulonFactory::fsm()
    {
        switch ( Keyboard->getKey() )
        {
        case MKeyboard::C_CLICK :
            return new MExit(Tools);
        case MKeyboard::P_CLICK :           // Так как последний в списке
            return new MExit(Tools);
        case MKeyboard::B_CLICK :
            Tools->clearAllKeys("qulon");    // Выбор заносится в энергонезависимую память
            return new MExit(Tools);
        default :;
        }

        return this;
    };

    // Завершение режима - до нажатия кнопки "С" удерживается индикация 
    MExit::MExit(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
        Display->getTextMode( (char*) "       OPTIONS:      " );
        Display->getTextHelp( (char*) "  C-RETURN TO SELECT " );
    }      
    MState * MExit::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                // Надо бы восстанавливать средствами диспетчера...
                Display->getTextMode( (char*) "      OPTIONS:       " );
                Display->getTextHelp( (char*) " U/D-OTHER  B-SELECT " );
                return nullptr;                             // Возврат к выбору режима
            default:;
        }
        return this;
    };


};

