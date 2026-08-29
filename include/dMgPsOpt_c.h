#ifndef DMGPSOPT_C_H
#define DMGPSOPT_C_H

#include "dThIcon_c.h"

/* The minigame pause/options touch controls embedded in dScMgBase_c. The ROM
 * constructs and destroys eight polymorphic TouchIcon_c elements as one member.
 * TouchIcon_c's nested name and dThIcon_c base are RTTI-proven. dThIcon_c's
 * ROM vtable proves this nested class has an ordinary nonvirtual destructor. */
struct dMgPsOpt_c {
    struct TouchIcon_c : dThIcon_c {
        TouchIcon_c();
        ~TouchIcon_c();
    };

    TouchIcon_c mIcons[8];
    s32 mSelectedIcon;
    u8 mActive;
    u8 pad_125[0x3];

    dMgPsOpt_c();
    ~dMgPsOpt_c();
};

typedef char dMgPsOpt_TouchIcon_c_size_must_be_0x24[
    sizeof(dMgPsOpt_c::TouchIcon_c) == 0x24 ? 1 : -1];
typedef char dMgPsOpt_c_size_must_be_0x128[
    sizeof(dMgPsOpt_c) == 0x128 ? 1 : -1];

#endif
