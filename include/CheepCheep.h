#ifndef CHEEPCHEEP_H
#define CHEEPCHEEP_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS proves this class as daPukupuku_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daPukupuku_c_classInit at 0x02133634 (historical alias CheepCheep_Spawn)
 * allocates 0x388 and installs this class's cartridge vtable. It backs the
 * PUKUPUKU registry profile, whose descriptor at 0x02134300 is reconstructed
 * as g_profile_PUKUPUKU.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct CheepCheep : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    u8  pad_370[0x4];
    s32 mHomePosX;                      /* 0x374 */
    s32 mHomePosY;                      /* 0x378 */
    s32 mHomePosZ;                      /* 0x37c */

    /* --- vtable --- */
    virtual ~CheepCheep();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Tail padding. The field span stops short of the real size: CheepCheep_Spawn
       calls fBase_c::operator new(0x388), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_380[0x8];      /* 0x380, to the ROM's 0x388 */
};

typedef char CheepCheep_size_must_be_0x388[sizeof(CheepCheep) == 0x388 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct CheepCheep {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x1a];
    u32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10CheepCheepD1Ev.c] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x110 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10CheepCheepD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x150 */
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x67];
    s32 mHomePosX;            /* 0x374 */
    s32 mHomePosY;            /* 0x378 */
    s32 mHomePosZ;            /* 0x37c */
};

#endif /* __cplusplus */

#endif /* CHEEPCHEEP_H */
