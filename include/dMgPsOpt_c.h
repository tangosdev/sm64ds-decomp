#ifndef DMGPSOPT_C_H
#define DMGPSOPT_C_H

#include "dThIcon_c.h"

/* Minigame pause / options touch panel. dScMgBase_c embeds one at 0xf4
 * as mTouchOptions: eight TouchIcon_c buttons. The nested name and the
 * dThIcon_c base are the cartridge's RTTI. Behavior is the base's;
 * Render is this class's. */
struct dMgPsOpt_c {
    struct TouchIcon_c : dThIcon_c {
        TouchIcon_c();
        ~TouchIcon_c();

        /* Vtable slot 1. Slot 0 stays dThIcon_c::Behavior. This has to
           be declared: without it the slot keeps the base Render. */
        void Render();
    };

    TouchIcon_c mIcons[8]; /* 0x000 */
    /* How many icons Behavior and Render walk. 8 while the panel is up,
       0 when it is shut. Not which icon is selected. */
    s32 mIconCount;        /* 0x120 */
    /* 0 shut, 1 taking touches, 2 counting down the close. */
    u8 mActive;            /* 0x124 */
    /* Frames left in state 2. The back button loads 0x14. */
    u8 mCloseTimer;        /* 0x125 */
    u8 pad_126[0x2];       /* 0x126 */

    dMgPsOpt_c();
    ~dMgPsOpt_c();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgPsOpt_TouchIcon_c_size_must_be_0x24[
    sizeof(dMgPsOpt_c::TouchIcon_c) == 0x24 ? 1 : -1];
typedef char dMgPsOpt_c_size_must_be_0x128[
    sizeof(dMgPsOpt_c) == 0x128 ? 1 : -1];
#endif

#endif
