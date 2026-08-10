#ifndef CHEEPCHEEP_H
#define CHEEPCHEEP_H

#include "types.h"
#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

/* Derives from Enemy: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct CheepCheep : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;/* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    u8  pad_370[0x4];
    s32 unk_374;                      /* 0x374 */
    s32 unk_378;                      /* 0x378 */
    s32 unk_37c;                      /* 0x37c */

    /* --- vtable --- */
    virtual ~CheepCheep();

    int Behavior();
    int InitResources();
    int Render();
};

typedef char CheepCheep_size_must_be_0x380[sizeof(CheepCheep) == 0x380 ? 1 : -1];

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
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10CheepCheepD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10CheepCheepD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x150 */
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x67];
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
};

#endif /* __cplusplus */

#endif /* CHEEPCHEEP_H */
