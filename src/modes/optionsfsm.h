#ifndef _OPTIONSFSM_H_
#define _OPTIONSFSM_H_

#include "mstate.h"

namespace OptionFsm
{
    struct MOptConsts
    {
        // Примеры
        static constexpr float offsetV  = 0.0f;
        static constexpr float multV    = 1.0f;


        static constexpr int ppone_l =  0;        // задержка включения (Отложенный старт)
        static constexpr int ppone_h = 24;

        static constexpr float c_offset_h =  2.00f;
        static constexpr float c_offset_l = -2.00f;

        static constexpr float v_offset_h =  2.00f;
        static constexpr float v_offset_l = -2.00f;


    };

    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            virtual MState * fsm() override;
    };

    // class MSelectBattery : public MState
    // {
    //     public:     
    //         MSelectBattery(MTools * Tools);   
    //         virtual MState * fsm() override;
    // };

    class MSetPostpone : public MState
    {       
        public:
            MSetPostpone(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetCurrentOffset : public MState
    {
        public:
            MSetCurrentOffset(MTools * Tools);
            virtual MState * fsm() override;
    };

  class MSetVoltageOffset : public MState
    {
        public:
            MSetVoltageOffset(MTools * Tools);
            virtual MState * fsm() override;
    };




    class MSetDCSupplyFactory : public MState
    {
        public:
            MSetDCSupplyFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetCcCvChargeFactory : public MState
    {
        public:
            MSetCcCvChargeFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetQulonFactory : public MState
    {
        public:
            MSetQulonFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MExit : public MState
    {
        public:  
            MExit(MTools * Tools);   
            virtual MState * fsm() override;
    };

};

#endif // !_OPTIONSFSM_H_