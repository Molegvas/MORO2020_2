#ifndef _OPTIONSFSM_H_
#define _OPTIONSFSM_H_

#include "mstate.h"

namespace OptionFsm
{
    // Константы, ограничивающие пользовательские регулировки
    struct MOptConsts
    {
        // задержка включения (Отложенный старт)
        static constexpr int ppone_l =  0;
        static constexpr int ppone_h = 24;

        // смещение показаний по току
        static constexpr float c_offset_h =  2.00f;
        static constexpr float c_offset_l = -2.00f;

        // смещение показаний по напряжению
        static constexpr float v_offset_h =  2.00f;
        static constexpr float v_offset_l = -2.00f;

        //...
    };

    enum MODES
    {
      QULON = 0,         // 
      TEMPLATE,               // шаблон режима 
      DCSUPPLY,               // режим источника постоянного тока
      PULSEGEN,               // режим источника импульсного тока
      CCCVCHARGE,             // режим заряда "постоянный ток / постоянное напряжение"
      PULSECHARGE,            // режим импульсного заряда
      RECOVERY,               // режим восстановления
      STORAGE,                // режим хранения
      DEVICE,                 // режим заводских регулировок
      SERVICE                // режим Сервис АКБ
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

    class MNameSelection : public MState
    {
        public:
            MNameSelection(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetFactory : public MState
    {
        public:
            MSetFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

//***********
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

    class MSetExChargeFactory : public MState
    {
        public:
            MSetExChargeFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetRecoveryFactory : public MState
    {
        public:
            MSetRecoveryFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MServiceFactory : public MState
    {
        public:
            MServiceFactory(MTools * Tools);
            virtual MState * fsm() override;
    };

    class MSetQulonFactory : public MState
    {
        public:
            MSetQulonFactory(MTools * Tools);
            virtual MState * fsm() override;
    };
//***************

    class MExit : public MState
    {
        public:  
            MExit(MTools * Tools);   
            virtual MState * fsm() override;
    };

};

#endif // !_OPTIONSFSM_H_