#ifndef _EXCHARGERFSM_H_
#define _EXCHARGERFSM_H_

#include "mstate.h"

namespace ExChargerFsm
{
    struct MChConsts
    {
        // Константы для пересчета из номинальных параметров батареи 
        //в параметры заряда
        static constexpr float voltageMaxFactor     = 1.234f;    // 12v  * 1.234 = 14.8v
        static constexpr float voltageMinFactor     = 0.890f;    // 12v  * 0.89  = 10.7v
        static constexpr float currentMaxFactor     = 0.200f;    // 55ah * 0.1   = 5,5A 
        static constexpr float currentMinFactor     = 0.050f;    // 55ah * 0.05  = 2.75A

        // Предельные значения регулировки 
        // ...
    };

    struct MPidConstants
    {
        // Параметры регулирования
        static constexpr float outputMin            = 0.0f;
        static constexpr float outputMaxFactor      = 1.05f;     // factor for current limit
        static constexpr float bangMin              = 20.0f;     // За пределами, - отключить
        static constexpr float bangMax              = 20.0f;
        static constexpr unsigned long timeStep     = 100;
        // Подобранные значения для ПИД
        static constexpr float k_p                  = 0.13f;
        static constexpr float k_i                  = 0.10f;
        static constexpr float k_d                  = 0.04f;
    };

    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            virtual MState * fsm() override;
    };


    // ...


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

#endif // !_EXCHARGERFSM_H_
