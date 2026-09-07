/* class dScMgCurling_c : dScMgBase_c. Confirmed leaf -- no RTTI record
 * names it as a base (tools/rtti_extract.py). Its D1
 * (src/_ZN11dScMgCurling_cD1Ev.cpp) writes only its own vtable then calls
 * dScMgBase_c's D2 directly; no members here need explicit destruction.
 *
 * Fields below 0x4660 (dScMgBase_c's own asserted size) are INHERITED, not
 * dScMgCurling_c's own -- the old flat header's `unk_0a4` at 0x0a4 is
 * really dScMgBase_c's own field at that offset, misattributed by
 * tools/deepen_rtti.py's flat-struct generation (it doesn't know about
 * inheritance). SIZE NOT ASSERTED: a leaf, so nothing needs the number
 * (same reasoning include/Stage.h documents). */
#ifndef DSCMGCURLING_C_H
#define DSCMGCURLING_C_H
#include "dScMgBase_c.h"

/* One curling stone, 0x2c bytes, five of them at 0x4660. */
struct dScMgCurling_stone {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u8  unk08[0x21];    /* 0x08 */
    u8  active;         /* 0x29 */
    u8  unk2a[0x2];     /* 0x2a */
};

typedef char dScMgCurling_stone_size_must_be_0x2c[sizeof(struct dScMgCurling_stone) == 0x2c ? 1 : -1];

struct dScMgCurling_c : dScMgBase_c {
    /* Declared, not defined inline -- a leaf, so nothing needs to inline
       it; real body in src/_ZN11dScMgCurling_cD1Ev.cpp / _D0Ev.cpp. */
    virtual ~dScMgCurling_c();

    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    /* Slot 18 (one of dScMgBase_c's own undeclared new slots 18-35) is
       left unnamed here too, same reasoning as dScMgBase_c.h's own -- its
       target (src/func_ov006_020e3470.cpp) has a "recovered name" of
       OnYoshiTryEat_020e3470, which is wrong (same tree-wide mislabel
       documented in notes/dscene-c-siblings-census.md section 3): its
       body sets fields and calls helpers, nothing like a destructor. */

    dScMgCurling_stone mStone[5];   /* 0x4660, stride 0x2c */
    u8  pad_473c[0x770];
    s32 unk_4eac;            /* 0x4eac */
    u8  pad_4eb0[0x28];
    s32 unk_4ed8;            /* 0x4ed8 */
    u8  pad_4edc[0x4];
    u16 unk_4ee0;            /* 0x4ee0 */
    u16 unk_4ee2;            /* 0x4ee2 */
    u8  pad_4ee4[0x3];
    u8  unk_4ee7;            /* 0x4ee7 */
    /* trailing extent the ROM's `new dScMgCurling_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_4ee8[0x4];
};

typedef char dScMgCurling_c_size_must_be_0x4eec[sizeof(struct dScMgCurling_c) == 0x4eec ? 1 : -1];

#endif
