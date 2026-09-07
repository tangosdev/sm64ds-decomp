#ifndef DSCMGPACHINKO2_C_H
#define DSCMGPACHINKO2_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko2_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper, not a declared method). Fields
   below dScMgBase_c's own 0x4660 are INHERITED, not this class's own --
   accessed via raw offsets on a char* cast of `this`, same as every other
   dScMgBase_c leaf (0xa4, 0xbc, 0xc0, 0xc3, 0xc4 all touched here, all
   already dScMgBase_c's own). Own fields observed only from slots
   0/6/9/16/17/18 directly; fields touched only by non-virtual helper
   functions (still raw extern "C" calls, not migrated methods) are not
   represented here.
 *
 * SM64DS RTTI names the implementation dScMgPachinko2_c. The reconstructed factory
 * dScMgPachinko2_c_classInit (historical alias MgLakituLaunch_Spawn) installs this class's
 * cartridge vtable for the MG_TAMAIRE registry profile.
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
typedef char dScMgPachinko2_Ball_size_must_be_0x40[sizeof(struct dScMgPachinko2_Ball) == 0x40 ? 1 : -1];

struct dScMgPachinko2_c : dScMgBase_c {
    virtual ~dScMgPachinko2_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    dScMgPachinko2_Ball mBalls[0x40]; /* 0x4660 -- 0x40 x 0x40; see func_ov006_02102fe8 */
    s32 unk_5660;            /* 0x5660 */
    u8  pad_5664[0x8];
    u16 unk_566c;            /* 0x566c */
    u16 unk_566e;            /* 0x566e */
    /* trailing extent the ROM's `new dScMgPachinko2_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_5670[0xc];
};

typedef char dScMgPachinko2_c_size_must_be_0x567c[sizeof(struct dScMgPachinko2_c) == 0x567c ? 1 : -1];

#endif
