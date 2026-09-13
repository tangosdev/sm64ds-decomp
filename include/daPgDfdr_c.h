#ifndef DAPGDFDR_C_H
#define DAPGDFDR_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Penguin Defender (PENGUIN_DEFENDER 258).
 *
 * RTTI ov027:0x021139e4 names the class daPgDfdr_c; the debug table at
 * 0x02090810 names the profile PENGUIN_DEFENDER. Base is dBgActor_c (RTTI
 * edge ov002:0x021089ec). ov027 is mixed (sliding ice / chill bully / Bubba
 * / snowman breath) -- this is the penguin, not those.
 *
 * SIZE 0x3dc, the literal daPgDfdr_c_classInit passes to operator new.
 * Factory constructs ModelAnim 0x320, TextureSequence 0x384, dCcAc_c 0x398
 * in that order; D1 tears them down in reverse. 0x398 + 0x34 = 0x3cc, and
 * 0x3cc..0x3dc is POD this TU writes:
 *
 *   0x3cc  mStateTable -- func_ov027_02111d70 writes
 *                     &data_ov027_02113ce4[idx]; 02111d38 / 02111cfc call
 *                     through entry [0] / [1] as this-call PMFs.
 *   0x3d0  unk_3d0 -- 02111ca8 stores 0 (turn), 02111b2c stores 1 (walk).
 *   0x3d4  mDistanceLeft -- 02111b2c loads data_ov027_02113a1c[mStepIndex];
 *                     02111a28 spends it against mHorzSpeed.
 *   0x3d8  mTimer -- 02111ca8 stores 0x14; 02111c48 feeds DecIfAbove0_Byte.
 *   0x3d9  mStepIndex -- 0..8, wraps at 9; indexes the stride-0xc table.
 *   0x3da  pad_3da -- never written in this TU.
 */

struct daPgDfdr_c : dBgActor_c {
    ModelAnim mModelAnim;                  /* 0x320 */
    TextureSequence mTextureSequence;      /* 0x384 */
    dCcAc_c mdCcAc_c;                      /* 0x398 */
    void *mStateTable;                     /* 0x3cc */
    s32   unk_3d0;                         /* 0x3d0 */
    s32   mDistanceLeft;                   /* 0x3d4 */
    u8    mTimer;                          /* 0x3d8 */
    u8    mStepIndex;                      /* 0x3d9 */
    u8    pad_3da[0x2];

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x021118c8 D1 then 0x02111924 D0) plus a D2 with no ROM
       home. Empty body: the three member teardowns, the vptr store and
       dBgActor_c's two member teardowns are synthesised. Key function is
       InitResources, the first declared non-inline virtual. */
    virtual ~daPgDfdr_c() {}

    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty in the ROM */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* slot 31 (Kill) is NOT overridden: vtable word at data_ov027_02113a90
       + 0x7c is 0x020ee55c, identical to dBgActor_c's own slot 31. */
};

typedef char daPgDfdr_c_size_must_be_0x3dc[sizeof(daPgDfdr_c) == 0x3dc ? 1 : -1];

#endif /* __cplusplus */

#endif
