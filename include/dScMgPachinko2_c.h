#ifndef DSCMGPACHINKO2_C_H
#define DSCMGPACHINKO2_C_H
#include "dScMgBase_c.h"

/* The Tamaire minigame scene: scene 0x171, profile MG_TAMAIRE. RTTI derives
 * dScMgPachinko2_c directly from dScMgBase_c, and no RTTI record names it as
 * a base. It overrides vtable slots 0 (InitResources), 6 (Behavior),
 * 9 (Render), 16 and 17 (the destructors) and 18 (OnYoshiTryEat). Offsets
 * below 0x4660 belong to dScMgBase_c. Own offsets that only the unnamed
 * helper functions touch are still padding.
 *
 * The factory dScMgPachinko2_c_classInit (historical alias
 * MgLakituLaunch_Spawn) installs this class's vtable. The ROM proves the
 * class; the classInit spelling follows later EAD lineage.
 */
/* One launched ball. func_ov006_02102fe8 drives it from the touch record while
   the pen is down (x/y follow the pen, px/py keep the pen offset) and, on
   release, aims it at (0x80, 0x20): angle = atan2, speed from the distance,
   vx/vy from the sine table. */
struct dScMgPachinko2_Ball {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 */
    s32 vx;           /* +0x08 */
    s32 vy;           /* +0x0c */
    s32 px;           /* +0x10 -- pen offset while held */
    s32 py;           /* +0x14 */
    u8  unk_18[0x8];
    s32 speed;        /* +0x20 */
    u8  unk_24[0x4];
    s32 sound;        /* +0x28 -- Sound_PlayIfNotActive handle */
    s32 prevDist;     /* +0x2c */
    u8  unk_30[0x2];
    s16 unk_32;       /* +0x32 */
    s16 angle;        /* +0x34 */
    s16 unk_36;       /* +0x36 */
    u8  unk_38;
    u8  state;        /* +0x39 -- 2 while released, 0 when reset */
    u8  unk_3a[0x6];
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko2_Ball_size_must_be_0x40[sizeof(struct dScMgPachinko2_Ball) == 0x40 ? 1 : -1];
#endif

struct dScMgPachinko2_c : dScMgBase_c {
    virtual ~dScMgPachinko2_c();  /* slots 16 and 17 */
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg); /* slot 18 */

    dScMgPachinko2_Ball mBalls[0x40]; /* 0x4660 -- 0x40 x 0x40; see func_ov006_02102fe8 */
    s32 unk_5660;            /* 0x5660 -- Behavior switches on it, 0..3 */
    u8  pad_5664[0x8];
    u16 unk_566c;            /* 0x566c */
    u16 unk_566e;            /* 0x566e -- set to 0x40; Behavior's case 3 counts it down */
    /* the factory allocates 0x567c bytes; see tools/opnew_sizes.py */
    u8 pad_5670[0xc];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko2_c_size_must_be_0x567c[sizeof(struct dScMgPachinko2_c) == 0x567c ? 1 : -1];
#endif

#endif
