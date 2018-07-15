#ifndef  _GAMEINPUT_H
#define  _GAMEINPUT_H

#include "virtualKeysGame.h"

struct ModifierKeysStates
{
    ModifierKeysStates(bool shiftState, bool ctrlState, bool altState)
    {
        bShift = shiftState;
        bCtrl = ctrlState;
        bAlt = altState;
    }

    bool bShift;
    bool bCtrl;
    bool bAlt;
};


#endif  //_GAMEINPUT_H
