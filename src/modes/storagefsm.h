#ifndef _STORAGEFSM_H_
#define _STORAGEFSM_H_

#include "mstate.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/moverseer.h"
#include "display/mdisplay.h"


namespace StorageFsm
{
    struct MChConsts
    {
        // Пределы регулирования
        static constexpr float i_l =  0.2f;
        static constexpr float i_h = 12.2f;
        static constexpr float v_l = 10.0f;
        static constexpr float v_h = 16.0f;


        // // Параметры условий заряда (здесь – для батарей типа AGM)
        // static constexpr float voltageMaxFactor     = 1.234f;    // 12v  * 1.234 = 14.8v
        // static constexpr float voltageMinFactor     = 0.890f;    // 12v  * 0.89  = 10.7v
        // static constexpr float currentMaxFactor     = 0.200f;    // 55ah * 0.1   = 5,5A 
        // static constexpr float currentMinFactor     = 0.050f;    // 55ah * 0.05  = 2.75A
    };



    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetCurrentDis : public MState
    {
        public:     
            MSetCurrentDis(MTools * Tools);
            virtual MState * fsm() override;
    };
       
    class MSetVoltageMin : public MState
    {
        public:   
            MSetVoltageMin(MTools * Tools);
            virtual MState * fsm() override;
    };
      
    class MExecution : public MState
    {
     public:   
        MExecution(MTools * Tools); // : MState(Tools) {
//             //Tools->clrAhCharge();                  // Обнуляются счетчики времени и отданного заряда

//             // Задаются начальные напряжение и ток
//             Board->setVoltageVolt( 0.0f );        //Tools->getVoltageMax() );         // Voltage limit
//             Board->setCurrentAmp( 0.0f );
//             Board->setDischargeAmp( Tools->getCurrentDis() );  // 
//             Board->powOn();   Board->swOn();                    // Включение преобразователя и коммутатора. 
//             #ifdef V22
//                 Board->ledsGreen();                  // Зеленый светодиод - процесс разряда запущен
//             #endif
//             // Задание отображения на экране дисплея построчно (4-я строка - верхняя)
//             // Oled->showLine4RealVoltage();
//             // Oled->showLine3RealCurrent();
//             // Oled->showLine2Text(" Идёт разряд... ");            // 
//             // Oled->showLine1Time( Tools->getChargeTimeCounter() );
//             // //Oled->showLine1Ah( Tools->getAhCharge() );
//             // Oled->showLine1Celsius( Board->Overseer->getCelsius() );


//     //      Tools->setSetPoint( Tools->getVoltageMax() );

//         //   #ifdef DEBUG_SUPPLY
//         //       Serial.println(" Источник питания стартовал с параметрами:");
//         //       Serial.print("  Voltage max, B  *** = ");  Serial.println( Tools->getVoltageMax() );
//         //       Serial.print("  Current max, A  *** = ");  Serial.println( Tools->getCurrentMax() );
//         //   #endif

// //                                                            // задающей напряжение цепи

//        }     
        virtual MState * fsm() override;
    };



    // class MStop : public MState
    // {
    //  public:  
    //     MStop(MTools * Tools) : MState(Tools) {
    //         Tools->shutdownCharge();            // Включение красного светодиода здесь
    //     }      
    //     virtual MState * fsm() override;
    // };
    
    class MStop : public MState
    {
        public:  
            MStop(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MExit : public MState
    {
        public:
            MExit(MTools * Tools);
            virtual MState * fsm() override;
    };

};

#endif // !_STORAGEFSM_H_