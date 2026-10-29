#ifndef DAPGDFDR_C_H
#define DAPGDFDR_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dExtAnmModel_c.h"
#include "dExtAnmTexPat_c.h"
#include "dCcAc_c.h"

/* Penguin Defender (PENGUIN_DEFENDER 258).
 *
 * RTTI ov027:0x021139e4 names the class daPgDfdr_c; the debug table at
 * 0x02090810 names the profile PENGUIN_DEFENDER. Base is dBgActor_c (RTTI
 * edge ov002:0x021089ec). ov027 is mixed (sliding ice / chill bully / Bubba
 * / snowman breath) -- this is the penguin, not those.
 *
 * SIZE 0x3dc, the literal daPgDfdr_c_classInit passes to operator new.
 * Factory constructs dExtAnmModel_c 0x320, dExtAnmTexPat_c 0x384, dCcAc_c 0x398
 * in that order; D1 tears them down in reverse. 0x398 + 0x34 = 0x3cc, and
 * 0x3cc..0x3dc is POD this TU writes:
 *
 *   0x3cc  mStateTable -- SetState writes &data_ov027_02113ce4[idx];
 *                     CallStateInit / CallStateBehavior call through entry
 *                     [0] / [1] as this-call PMFs.
 *   0x3d0  mIsWalking -- InitStateTurn stores 0, InitStateWalk stores 1.
 *                     Nothing in this TU reads it.
 *   0x3d4  mDistanceLeft -- InitStateWalk loads data_ov027_02113a1c[mStepIndex];
 *                     StateWalk spends it against mHorzSpeed.
 *   0x3d8  mTimer -- InitStateTurn stores 0x14; StateTurn feeds DecIfAbove0_Byte.
 *   0x3d9  mStepIndex -- 0..8, wraps at 9; indexes the stride-0xc table.
 *   0x3da  pad_3da -- never written in this TU.
 */

struct daPgDfdr_c : dBgActor_c {
    dExtAnmModel_c mAnmModel;         /* 0x320 */
    dExtAnmTexPat_c mTextureSequence; /* 0x384 */
    dCcAc_c mdCcAc_c;                 /* 0x398 */
    void *mStateTable;                /* 0x3cc */
    s32 mIsWalking;                   /* 0x3d0 */
    s32 mDistanceLeft;                /* 0x3d4 */
    u8 mTimer;                        /* 0x3d8 */
    u8 mStepIndex;                    /* 0x3d9 */
    u8 pad_3da[0x2];

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x021118c8 D1 then 0x02111924 D0) plus a D2 with no ROM
       home. Empty body: the three member teardowns, the vptr store and
       dBgActor_c's two member teardowns are synthesised. Key function is
       InitResources, the first declared non-inline virtual. */
    virtual ~daPgDfdr_c() {}

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
    virtual void OnPendingDestroy(); /* slot 12 -- empty in the ROM */

    void CallStateBehavior(); /* ov027 0x02111cfc */
    void CallStateInit();     /* ov027 0x02111d38 */
    void SetState(int idx);   /* ov027 0x02111d70 */
    int StateWalk();          /* ov027 0x02111a28 */
    void UpdateModelMatrix(); /* ov027 0x02111994 */
    void InitStateWalk();     /* ov027 0x02111b2c */
    int StateTurn();          /* ov027 0x02111c48 */
    int InitStateTurn();      /* ov027 0x02111ca8 */

    /* slot 31 (Kill) is NOT overridden: vtable word at data_ov027_02113a90
       + 0x7c is 0x020ee55c, identical to dBgActor_c's own slot 31. */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPgDfdr_c_size_must_be_0x3dc[sizeof(daPgDfdr_c) == 0x3dc ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
