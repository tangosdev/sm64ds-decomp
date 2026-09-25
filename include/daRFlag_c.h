/**
 * Race flag. While armed, Mario touching mdCcAc_c ends the course timer.
 *
 * The name is the cartridge's: _ZTI9daRFlag_c at ov062 0x0211dc24
 * (__si_class_type_info -> dActor_c) and _ZTV9daRFlag_c at 0x0211dc54
 * with exactly the 31 dActor_c slots. The factory allocates 0x174 bytes
 * and constructs dCcAc_c at +0xd4 and ModelAnim at +0x108.
 */
#ifndef DARFLAG_C_H
#define DARFLAG_C_H
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct daRFlag_c : dActor_c {
    u8 pad_0d0[0x4];
    dCcAc_c mdCcAc_c;       /* 0x0d4 touch cylinder */
    ModelAnim mModelAnim;   /* 0x108 */
    u16 mVictoryTimer;      /* 0x16c frames since Mario touched the flag */
    /* 0xff from InitResources (disarmed); daRNk_c writes 0 when the race
       starts; 1 once Mario (Behavior) or Koopa the Quick (daRNk_c) reaches
       the flag. Koopa reads it to decide who won. */
    u8 mHasTouchedFlag;     /* 0x16e */
    u8 pad_16f[0x5];        /* nothing in this class reads these */
    virtual ~daRFlag_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daRFlag_c_size_must_be_0x174[sizeof(struct daRFlag_c) == 0x174 ? 1 : -1];
#endif

#endif
