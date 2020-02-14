#ifndef _RECOVERYFSM_H_
#define _RECOVERYFSM_H_

#include "mstate.h"

namespace RecoveryFsm
{
    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            virtual MState * fsm() override;
    };

    // ... другие состояния

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

#endif // !_RECOVERYFSM_H_