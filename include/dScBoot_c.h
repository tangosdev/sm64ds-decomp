#ifndef DSCBOOT_C_H
#define DSCBOOT_C_H

#include "dScene_c.h"

/* The boot/intro scene: fBase_c -> dBase_c -> dScene_c -> dScBoot_c
 * (the ROM's own type graph spells it dScBoot_c). A leaf -- nothing derives
 * from it.
 *
 * It draws the title/language screen and runs the two-button menu that can
 * erase all save data. Derivation, vtable and layout evidence:
 * notes/scene-provenance.md.
 *
 * Field NAMES cannot change codegen. Offsets and widths are observed.
 */
struct dScBoot_c : dScene_c {
    u16 mFadeTimer;         /* 0x050 -- frames left before the scene fades out */
    u8  mState;             /* 0x052 -- menu state machine; see Behavior */
    u8  mSelectedButton;    /* 0x053 -- 0 = language, 1 = erase */
    u8  mButtonFlashTimer;  /* 0x054 -- suppresses the highlight while it runs */
    u8  mInputLockTimer;    /* 0x055 -- swallows input after a press */
    u8  mEraseEffectTimer;  /* 0x056 -- "erase all save data" countdown */
    u8  pad_057[0x1];

    /* Declared first, deliberately: that makes ~dScBoot_c the key function,
       and it is defined out of line in src/named/arm9/d_s_boot.cpp, which therefore emits
       the vtable/typeinfo group. */
    virtual ~dScBoot_c();

    virtual s32 InitResources();          /* slot 0 */
    virtual s32 Behavior();               /* slot 6 */

    /* The button-row redraw at 0x02005348: reads mSelectedButton/mButtonFlashTimer
       and repaints the two touch buttons' palette bits on the sub screen. Its
       only caller is Behavior, always on `this`. Address-derived label kept as
       the method name (S33); symbols.txt carries _ZN9dScBoot_c13func_02005348Ev. */
    void func_02005348();
};

/* Holds the chain to the size dScBoot_c_classInit's operator new(0x58) evidences.
   A silently-added member anywhere fails this. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScBoot_c_size_must_be_0x58[sizeof(dScBoot_c) == 0x58 ? 1 : -1];
#endif

#endif
