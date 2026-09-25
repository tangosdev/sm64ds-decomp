#ifndef DSCMGBOMROOM_C_H
#define DSCMGBOMROOM_C_H
#include "dScMgBase_c.h"

/* Bob-omb sorting minigame (dScMgBomroom_c : dScMgBase_c). Fields below
 * 0x4660 belong to the base and stay raw; the bombs themselves start there.
 *
 * The name is the ROM's RTTI name, and rtti_extract.py confirms it is a
 * leaf (no RTTI record names it as a base). dScMgBomroom_c_classInit (alias
 * MgSortOrSplode_Spawn) installs its vtable for the MG_BOMROOM profile.
 */
/* One bomb: 0x70 records at +0x4660, stride 0x40. grabX/grabY are the
   stylus offset at the grab; speed is the Fix12 step along angle. */
struct dScMgBomroom_Bomb {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 -- Fix12 */
    s32 grabX;        /* +0x08 -- stylus x minus bomb x, Fix12 */
    s32 grabY;        /* +0x0c -- stylus y minus bomb y, Fix12 */
    s32 speed;        /* +0x10 -- Fix12 per-frame step; scaled by sin/cos of
                                   angle while roaming, added directly when
                                   walking to its slot */
    u8  unk_14[0x10];
    s32 sound;        /* +0x24 -- sound handle; func_02012468 takes it back
                                   and returns it each frame */
    u8  unk_28[4];
    u16 angle;        /* +0x2c -- 0..0xffff, sine-table index >> 4 */
    u16 unk_2e;       /* +0x2e -- counter, see func_ov006_020d69b8 */
    u16 unk_30;       /* +0x30 */
    u8  unk_32[0x4];
    u8  color;        /* +0x36 -- 0/1; also picks which pen box it bounces in */
    u8  state;        /* +0x37 -- 5 = settled */
    u8  active;       /* +0x38 */
    u8  unk_39[0x7];
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgBomroom_Bomb_size_must_be_0x40[sizeof(struct dScMgBomroom_Bomb) == 0x40 ? 1 : -1];
#endif

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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgBomroom_c_size_must_be_0x6300[sizeof(struct dScMgBomroom_c) == 0x6300 ? 1 : -1];
#endif

#endif
