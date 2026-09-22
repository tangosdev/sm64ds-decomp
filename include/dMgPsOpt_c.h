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

        /* Slot 1 of dThIcon_c's two-slot table, and this class's only override
           of it. The cartridge's _ZTVN10dMgPsOpt_c11TouchIcon_cE holds
           ov004:0x020b8c18 there where dThIcon_c's own table holds
           _ZN9dThIcon_c6RenderEv; without this declaration mwcc emitted the
           inherited address and romdata_check scored the table DIFFERS. Defined
           by _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv at ov004:0x020b8c18, in the
           class's own translation unit. Behavior (slot 0) is NOT overridden --
           the cartridge keeps dThIcon_c's there. */
        void Render();
    };

    TouchIcon_c mIcons[8];   /* 0x000..0x120 */
    s32 mSelectedIcon;       /* 0x120 */
    u8 mActive;              /* 0x124 */
    /* Frames left on the close delay. The back button (icon 5) loads 0x14 here
       and moves mActive to 2; state 2 counts it down and closes at zero. The
       byte is live, not padding -- ov004:0x020b8f78 both stores and decrements
       it -- so it is named rather than absorbed into the trailing pad. */
    u8 mCloseTimer;          /* 0x125 */
    u8 pad_126[0x2];         /* 0x126 */

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
