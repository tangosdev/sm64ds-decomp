#ifndef MONTYMOLEROCK_H
#define MONTYMOLEROCK_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct MontyMoleRock : dEnemyBase_c {
    Model mModel;                     /* 0x110 */
    dCcAc_c mdCcAc_c;/* 0x160 */
    dBgCh_Actr mWithMeshClsn;       /* 0x194 */
    u8 mIsSmall;                       /* 0x350 */

    /* --- vtable --- */
    virtual ~MontyMoleRock();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 InitResources();
    virtual s32 Render();
};

typedef char MontyMoleRock_size_must_be_0x354[sizeof(MontyMoleRock) == 0x354 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MontyMoleRock {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x6c];
    /* Model member, named by _ZN5ModelD1Ev at +0x110 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x110 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MontyMoleRockD1Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x194 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MontyMoleRockD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    u8  mIsSmall;            /* 0x350 */
};

#endif /* __cplusplus */

#endif /* MONTYMOLEROCK_H */
