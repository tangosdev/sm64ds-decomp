/* class dScMgRoulette_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgRoulette_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is
 * MgMushroomRoulette_Spawn; kept as the symbol name for the factory
 * (already attributed), but the class itself takes its real ROM identity,
 * matching this tree's current convention.
 *
 * SIZE 0x5400, from MgMushroomRoulette_Spawn.cpp's own
 * `_ZN7fBase_cnwEj(0x5400)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64), same
 * as five siblings -- see include/dScMgMemory_c.h's own note.
 *
 * ONE ARRAY at 0x51a8, 5 elements of 0x34 bytes each -- destructor calls
 * `__destroy_arr(p+0x51a8, 5, 0x34, func_ov006_021079c8)`, matching the
 * factory's own construction. Element type not evidenced -- raw bytes.
 *
 * 0x52ac..0x531c (0x70 bytes): touched only via raw offset calls
 * (func_ov006_02107b70(c+0x52ac), func_ov006_02108524(this+0x530c),
 * func_ov006_02107d80/021085c0(c+0x530c)), never through a named field --
 * left as pad, same reasoning as dScMgBase_c.h's own touchIcon note.
 *
 * TWO MODELS immediately after, 0x50 bytes each (include/Model.h): the
 * destructor (func_ov006_0210788c, pre-migration) destroys 0x536c THEN
 * 0x531c, THEN the array, THEN the table -- Itanium auto-destruction
 * would run typed members in reverse DECLARATION order strictly AFTER
 * the user body, which can't reproduce this interleaving (array/table
 * destruction sandwiched functionally between the two models' declared
 * order). Left as raw bytes with explicit destructor calls in the
 * measured order instead: a typed member here would cost an auto-generated
 * call the body's own explicit ordering contradicts. (This note used to
 * cite include/MgBounceAndPounce.h's own mModel field for the same
 * reasoning. That field turned out to belong to dScMgJump_c rather than to
 * the base, and left with it when the base was renamed dScMgD3DBase_c; the
 * reasoning stands on its own.)
 *
 * OWN TAIL, 0x53bc..0x5400: sixteen fields are real matched access
 * (src files 021095cc.c, 02109834.c, 0210a194.cpp -- dScMgRoulette_c's
 * own vtable methods).
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN15dScMgRoulette_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGROULETTE_C_H
#define DSCMGROULETTE_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_021079c8(void);
extern "C" void _ZN5ModelD1Ev(void *);

struct dScMgRoulette_c : dScMgSingle3DBase_c {
    virtual ~dScMgRoulette_c();

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- src/_ZN15dScMgRoulette_c13InitResourcesEv.cpp */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x0210980c */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x02109aac */
    virtual s32 Render();            /* slot 9 -- ov006 0x02109834 */

    u8    mTable[0x270];  /* 0x4f38 -- ctor func_ov006_020c1d80, dtor func_ov006_020c1c64 */
    u8    mArray[0x104];   /* 0x51a8 -- 5 * 0x34, dtor func_ov006_021079c8 */
    u8    pad_52ac[0x70];  /* 0x52ac -- no named field access, see file banner */
    u8    mModel1[0x50];    /* 0x531c -- Model, raw bytes, see file banner */
    u8    mModel2[0x50];    /* 0x536c -- Model, raw bytes, see file banner */
    u8    pad_53bc[0x8];     /* 0x53bc */
    s32   unk_53c4;          /* 0x53c4 -- while it is 0 the board is idle:
                                Render draws the tile cursor and phase 3 runs */
    u8    pad_53c8[0xe];
    s16   mSelectedTile;     /* 0x53d6 -- Render draws the cursor at that tile's
                                coordinates in data_ov006_02142ab4/ab8; Behavior
                                scores every racer against it */
    u8    pad_53d8[0x8];
    s32   unk_53e0;          /* 0x53e0 */
    s16   mCameraPreset;     /* 0x53e4 -- Render copies row n of the camera
                                tables (data_ov006_0213e34c / _0213e370 at stride
                                0xc, and _0213e2e0 for the angle) into
                                mCameraEye/mCameraTarget/mCameraAngle, then calls
                                Camera_UpdateMatrices. 0 leaves the camera alone */
    s16   mPhase;            /* 0x53e6 -- 1 deals the racers out, 2 runs the
                                countdown and reads the board, 3 pays out per
                                landed tile, 4 announces the result */
    s16   mPhaseTimer;       /* 0x53e8 -- counted down in every phase; each
                                phase reloads it on entry */
    s16   unk_53ea;          /* 0x53ea */
    u8    unk_53ec;          /* 0x53ec */
    u8    unk_53ed;          /* 0x53ed */
    u8    unk_53ee;          /* 0x53ee */
    u8    unk_53ef;          /* 0x53ef */
    u8    unk_53f0;          /* 0x53f0 */
    u8    pad_53f1[0x1];
    s16   mScore;            /* 0x53f2 -- the payout summed over the racers;
                                phase 4 compares it against mTargetScore */
    u8    pad_53f4[0x2];
    s16   mTargetScore;      /* 0x53f6 -- one per racer that missed the winning
                                tile; the bar mScore has to beat */
    s32   mDealIndex;        /* 0x53f8 -- how many racers have been dealt out;
                                also reused as a loop cursor between phases */
    s32   mRacerCount;       /* 0x53fc -- the bound of every per-racer loop in
                                Behavior and Render */
};

typedef char dScMgRoulette_c_size_must_be_0x5400[sizeof(dScMgRoulette_c) == 0x5400 ? 1 : -1];

#endif
