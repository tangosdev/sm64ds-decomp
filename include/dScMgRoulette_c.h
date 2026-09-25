/* dScMgRoulette_c -- Mushroom Roulette. Racers are dealt onto a spinning
 * board and paid by the tile they stop on. Child of dScMgSingle3DBase_c,
 * size 0x5400.
 *
 * RTTI: dScMgRoulette_c : dScMgSingle3DBase_c (tools/rtti_extract.py).
 * Factory dScMgRoulette_c_classInit; historical alias MgMushroomRoulette_Spawn.
 * ROM: D1 0x0210788c below D0 0x02107920, no D2; out of line would emit
 * D2, D0, D1. InitResources is then the first declared non-inline virtual,
 * so it is the key function; this TU licenses _ZTV/_ZTI/_ZTS in
 * compiler_only_output.
 *
 * The destructor is inline and declared first, so D1 comes out before D0
 * and no D2 is emitted. Its four calls follow the ROM: second model, first
 * model, racer array, table. Typed members would destroy in a different
 * order. InitResources stays the key function. dScMgBase_c supplies
 * operator delete.
 */
#ifndef DSCMGROULETTE_C_H
#define DSCMGROULETTE_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_021079c8(void);
extern "C" void _ZN5ModelD1Ev(void *);

struct dScMgRoulette_c : dScMgSingle3DBase_c {
    /* Inline and declared first: see the file banner. The four explicit calls
       are the ROM's own order -- mModel2, mModel1, mArray, mTable -- which
       typed members could not reproduce. */
    virtual ~dScMgRoulette_c() {
        _ZN5ModelD1Ev(mModel2);
        _ZN5ModelD1Ev(mModel1);
        __cxa_vec_cleanup(mArray, 5, 0x34, (void *)func_ov006_021079c8);
        func_ov006_020c1c64((char *)mTable);
    }

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- src/_ZN15dScMgRoulette_c13InitResourcesEv.cpp */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x0210980c */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x02109aac */
    virtual s32 Render();            /* slot 9 -- ov006 0x02109834 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */

    u8    mTable[0x270];   /* 0x4f38 -- the spinning board */
    u8    mArray[0x104];   /* 0x51a8 -- 5 racers, 0x34 each */
    u8    mMeter[4][0x18]; /* 0x52ac -- strips for payouts 12, 6, 3, 2 */
    u8    mSlider[0x10];   /* 0x530c -- PMF state and the two model files */
    u8    mModel1[0x50];   /* 0x531c -- raw Model; the destructor calls it explicitly */
    u8    mModel2[0x50];   /* 0x536c */
    u8    pad_53bc[0x8];   /* 0x53bc */
    s32   mBoardBusy;      /* 0x53c4 -- while 0 the board is idle: Render draws
                              the cursor and phase 3 pays out */
    u8    pad_53c8[0xe];   /* 0x53c8 */
    s16   mSelectedTile;   /* 0x53d6 -- cursor tile; phase 3 scores against it */
    s16   mTableReady;     /* 0x53d8 -- set once the table first reports ready */
    s16   pad_53da;        /* 0x53da */
    s32   mTilesScored;    /* 0x53dc -- set to 1 when phase 2 records the tiles */
    s32   unk_53e0;        /* 0x53e0 -- reset to 0x100; not read in this TU */
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
    s16   unk_53f4;          /* 0x53f4 -- cleared when the payout is posted */
    s16   mTargetScore;      /* 0x53f6 -- one per racer that missed the winning
                                tile; the bar mScore has to beat */
    s32   mDealIndex;        /* 0x53f8 -- how many racers have been dealt out;
                                also reused as a loop cursor between phases */
    s32   mRacerCount;       /* 0x53fc -- the bound of every per-racer loop in
                                Behavior and Render */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgRoulette_c_size_must_be_0x5400[sizeof(dScMgRoulette_c) == 0x5400 ? 1 : -1];
#endif

#endif
