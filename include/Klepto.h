#ifndef KLEPTO_H
#define KLEPTO_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN6KleptoD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first. Member NAMES are the ones this header already used --
 * a rebase should not also rename things its callers spell:
 *
 *     0x110 dCcAc_c       0x34   -> 0x144
 *     0x144 dCcAc_c       0x34   -> 0x178
 *     0x178 dBgCh_Actr             0x1bc  -> 0x334
 *     0x334 BlendModelAnim           0x70   -> 0x3a4
 *     0x3a4 ShadowModel              0x28   -> 0x3cc
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Klepto : dEnemyBase_c {
    dCcAc_c           mdCcAc_c1;  /* 0x110 */
    dCcAc_c           mdCcAc_c2;  /* 0x144 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x178 */
    BlendModelAnim               mBlendModelAnim;       /* 0x334 */
    ShadowModel                  mShadowModel;          /* 0x3a4 */
    u8  pad_3cc[0x60];
    s32                          unk_42c;               /* 0x42c */
    s32                          unk_430;               /* 0x430 */
    s32                          unk_434;               /* 0x434 */
    s32                          unk_438;               /* 0x438 */
    u8  pad_43c[0x8];
    s16                          unk_444;               /* 0x444 */
    u8  pad_446[0x2];
    u8                           unk_448;               /* 0x448 */
    u8  pad_449[0x1];
    s16                          unk_44a;               /* 0x44a */
    s32                          mHeldActorID;          /* 0x44c */
    s32                          unk_450;               /* 0x450 */
    s32                          unk_454;               /* 0x454 */
    s32                          unk_458;               /* 0x458 */
    u8  pad_45c[0x8];
    s32                          mPathId;               /* 0x464 */
    s32                          mCarriedItem;          /* 0x468 */
    s32                          unk_46c;               /* 0x46c */
    s32                          unk_470;               /* 0x470 */
    u32                          unk_474;               /* 0x474 */
    u8  pad_478[0xc];
    s32                          unk_484;               /* 0x484 */
    s32                          unk_488;               /* 0x488 */
    s32                          unk_48c;               /* 0x48c */

    /* --- vtable --- */
    virtual ~Klepto();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Klepto_size_must_be_0x490[sizeof(Klepto) == 0x490 ? 1 : -1];

#endif /* KLEPTO_H */
