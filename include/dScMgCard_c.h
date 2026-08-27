/* class dScMgCard_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgCard_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgPicturePoker_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x539c, from MgPicturePoker_Spawn.cpp's own
 * `_ZN7fBase_cnwEj(0x539c)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64), same
 * as five siblings -- see include/dScMgMemory_c.h's own note. TWO fields
 * fall WITHIN it and are real matched access (src/func_ov006_020db9dc.c):
 * unk_4f52 and unk_511e are literally the ctor's own `*(short*)(t+0x1a)=0`
 * and `*(short*)(t+0x1e6)=0` writes (func_ov006_020c1d80's own body), same
 * as dScMgMCarlo2_c's own instance of these two fields -- named here
 * instead of folded into the opaque table, everything else stays pad.
 *
 * TWO ARRAYS, 5 elements of 0x30 bytes each, immediately after: the
 * destructor destroys them in reverse order (0x5298 first, then 0x51a8),
 * matching two DIFFERENT element dtors (func_ov006_020d96f0,
 * func_ov006_020d96e0) -- two distinct element types, neither evidenced
 * by any direct field access, left as raw bytes.
 *
 * OWN TAIL, 0x5388..0x539c: nine fields are real matched access (five
 * src files: func_ov006_020da9c4.cpp, 020db6ec.c, 020db720.c,
 * 020db9dc.c, 020dbaf0.cpp -- five of dScMgCard_c's own vtable methods).
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/minigames/dScMgCard_c/_ZN11dScMgCard_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGCARD_C_H
#define DSCMGCARD_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020d96e0(void);
extern "C" void func_ov006_020d96f0(void);

struct dScMgCard_c : dScMgSingle3DBase_c {
    virtual ~dScMgCard_c();

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- src/minigames/dScMgCard_c/_ZN11dScMgCard_c13InitResourcesEv.cpp */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x020da994 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x020dabec */
    virtual s32 Render();            /* slot 9 -- src/minigames/dScMgCard_c/_ZN11dScMgCard_c6RenderEv.cpp */

    u8  pad_4f38[0x1a];   /* 0x4f38 -- shared table start, see file banner */
    s16 unk_4f52;          /* 0x4f52 -- within shared table */
    u8  pad_4f54[0x1ca];   /* 0x4f54 */
    s16 unk_511e;           /* 0x511e -- within shared table */
    u8  pad_5120[0x88];     /* 0x5120 -- rest of shared table (ends 0x51a8) */
    u8  mArray1[0xf0];      /* 0x51a8 -- 5 * 0x30, dtor func_ov006_020d96e0 */
    u8  mArray2[0xf0];      /* 0x5298 -- 5 * 0x30, dtor func_ov006_020d96f0 */
    s16 mState;              /* 0x5388 -- the round's step counter; the state
                                machine in src/unnamed/ov006/020d/func_ov006_020dac34.cpp is one
                                long switch on it and mostly just `++`s it */
    s16 mStateTimer;         /* 0x538a -- reloaded (0x10, 0x14, 0x1e, 0x3c,
                                0x5a) on each step and run down to 0 before
                                mState advances */
    s16 unk_538c;            /* 0x538c */
    s16 unk_538e;            /* 0x538e -- the mArray1 (0x51a8) highlight pair:
                                Render blinks the card whose config byte equals
                                either of the two, and draws every other card
                                every frame. 6 means "none" */
    s16 unk_5390;            /* 0x5390 -- cleared back to 6 when the two banks
                                disagree, while unk_538e keeps its value */
    s16 unk_5392;            /* 0x5392 -- the same pair for mArray2 (0x5298) */
    s16 unk_5394;            /* 0x5394 */
    s16 mFrameCounter;       /* 0x5396 -- Behavior's only own statement is
                                `+= 1`; Render blinks on bit 3 */
    s16 mScore;              /* 0x5398 -- a high-water mark of dScMgBase_c's own
                                0xb4, pushed to the HUD counter every frame */
    s8  unk_539a;             /* 0x539a */
    u8  pad_539b[0x1];        /* 0x539b */
};

typedef char dScMgCard_c_size_must_be_0x539c[sizeof(dScMgCard_c) == 0x539c ? 1 : -1];

#endif
