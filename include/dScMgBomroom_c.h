#ifndef DSCMGBOMROOM_C_H
#define DSCMGBOMROOM_C_H
#include "dScMgBase_c.h"

/* dScMgBomroom_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper, not a declared method). Fields
   below dScMgBase_c's own 0x4660 are INHERITED, not this class's own --
   accessed via raw offsets on a char* cast of `this`, same as every other
   dScMgBase_c leaf. Own fields observed only from slots 0/6/9/16/17/18
   directly; the large block of fields touched only by non-virtual helper
   functions (func_ov006_020d5b00 and friends, still raw extern "C" calls,
   not migrated methods) is not represented here -- it stays inside the
   leading pad.
 *
 * SM64DS RTTI names the implementation dScMgBomroom_c. The reconstructed factory
 * dScMgBomroom_c_classInit (historical alias MgSortOrSplode_Spawn) installs this class's
 * cartridge vtable for the MG_BOMROOM registry profile.
 */
/* One bomb (0x70 of them at 0x4660, stride 0x40, see func_ov006_020d6784's
   i < 0x70 walk). func_ov006_020d7c4c steps it along its angle at unk_10 per
   frame and bounces it off the room walls. */
struct dScMgBomroom_Bomb {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 -- Fix12 */
    u8  unk_08[0x8];
    s32 unk_10;       /* +0x10 -- speed, Fix12 per frame */
    u8  unk_14[0x18];
    u16 angle;        /* +0x2c -- 0..0xffff, sine-table index >> 4 */
    u16 unk_2e;       /* +0x2e -- counter, see func_ov006_020d69b8 */
    u16 unk_30;       /* +0x30 */
    u8  unk_32[0x4];
    u8  unk_36;       /* +0x36 -- inner-room flag: selects the wall box */
    u8  unk_37;       /* +0x37 -- state, 5 = settled */
    u8  unk_38;       /* +0x38 -- active */
    u8  unk_39[0x7];
};
typedef char dScMgBomroom_Bomb_size_must_be_0x40[sizeof(struct dScMgBomroom_Bomb) == 0x40 ? 1 : -1];

struct dScMgBomroom_c : dScMgBase_c {
    virtual ~dScMgBomroom_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    dScMgBomroom_Bomb mBombs[0x70]; /* 0x4660 -- 0x70 x 0x40 */
    u8  pad_6260[0x70];
    s32 unk_62d0;            /* 0x62d0 */
    u8  pad_62d4[0x1a];
    u16 unk_62ee;            /* 0x62ee */
    u16 unk_62f0;            /* 0x62f0 */
    /* trailing extent the ROM's `new dScMgBomroom_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_62f4[0xc];
};

typedef char dScMgBomroom_c_size_must_be_0x6300[sizeof(struct dScMgBomroom_c) == 0x6300 ? 1 : -1];

#endif
