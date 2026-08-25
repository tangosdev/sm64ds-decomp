/* class dScMgTrampoline_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgTrampoline_c : dScMgD3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable is ov006:0x0213fb34.
 *
 * SIZE 0x5dc8, from MgTrampolineTime_Spawn's own
 * `_ZN7fBase_cnwEj(0x5dc8)`. That factory writes dScMgD3DBase_c's vtable,
 * then this class's, then the three arrays below; the English _Spawn name
 * is already attributed and is kept.
 *
 * FACTORY AND DESTRUCTOR AGREE MEMBER FOR MEMBER, in exact reverse: built
 * 0x500c, 0x534c, 0x5cd0; destroyed 0x5cd0, 0x534c, 0x500c. No Model and no
 * typed member of any kind, so nothing here is compiler-generated -- the
 * destructor body is all three calls and nothing else.
 *
 * OWN TAIL, 0x5d94..0x5dc6: eighteen fields carried over verbatim from this
 * header's previous auto-generated form, at unchanged offsets and widths.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf. No separate operator
 * delete is needed: dScMgD3DBase_c, the immediate base, provides one. */
#ifndef DSCMGTRAMPOLINE_C_H
#define DSCMGTRAMPOLINE_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020ccfc8(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_02120938(void);

struct dScMgTrampoline_c : dScMgD3DBase_c {
    virtual ~dScMgTrampoline_c();

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- src/_ZN17dScMgTrampoline_c13InitResourcesEv.cpp */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x021212e0 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x021214f8 */
    virtual s32 Render();            /* slot 9 -- ov006 0x021212fc */

    u8  pad_5004[0x8];    /* 0x5004 -- no matched access in this class's methods */
    u8  mArray1[0x340];   /* 0x500c -- 4 * 0xd0,  elem dtor func_ov006_020ccfc8 */
    u8  mArray2[0x984];   /* 0x534c -- 3 * 0x32c, elem dtor func_ov006_020d1008 */
    u8  mArray3[0xb4];    /* 0x5cd0 -- 5 * 0x24,  elem dtor func_ov006_02120938 */
    u8  pad_5d84[0x10];   /* 0x5d84 */
    s32 mScrollY;         /* 0x5d94 -- approached toward mScrollTargetY by 2 a
                             tick; the BG2 offset is mScrollY + mScrollOffsetY */
    s32 mScrollTargetY;   /* 0x5d98 */
    s32 mScrollHoldTimer; /* 0x5d9c -- 0x78, run down to 0 before the target
                             may move again */
    s32 mScrollOffsetY;   /* 0x5da0 -- added to mScrollY everywhere it is used */
    s32 mArrow1X;         /* 0x5da4 -- the two guide arrows Render draws at
                             n + 0xf0; each slides between 0 and 0x20 */
    s32 mArrow2X;         /* 0x5da8 */
    u8  pad_5dac[0x4];    /* 0x5dac -- the drag sound handle */
    s16 mTouchX;          /* 0x5db0 -- the current stylus sample */
    s16 mTouchY;          /* 0x5db2 */
    s16 mTouchStartX;     /* 0x5db4 -- copied from mTouch on the press edge and
                             left alone; the swipe is measured against it */
    s16 mTouchStartY;     /* 0x5db6 */
    s16 mInputEnabled;    /* 0x5db8 -- while 0 the touch handler clears
                             mTouching and returns at once */
    s16 unk_5dba;         /* 0x5dba */
    s16 unk_5dbc;         /* 0x5dbc */
    s16 unk_5dbe;         /* 0x5dbe */
    s16 unk_5dc0;         /* 0x5dc0 */
    s16 unk_5dc2;         /* 0x5dc2 */
    u8  mTouching;        /* 0x5dc4 -- a drag is in progress */
    u8  mTouchReleased;   /* 0x5dc5 -- set on the release edge; the swipe test
                             in src/func_ov006_0212101c.c consumes and clears it */
    u8  pad_5dc6[0x2];    /* 0x5dc6 -- rounds up to the 0x5dc8 boundary */
};

typedef char dScMgTrampoline_c_size_must_be_0x5dc8[sizeof(dScMgTrampoline_c) == 0x5dc8 ? 1 : -1];

#endif
