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
 *
 * SM64DS RTTI names the implementation daJango_c. The reconstructed factory
 * daJango_c_classInit (historical alias Klepto_Spawn)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_JANGO (historical alias Klepto_SpawnInfo)
 * is its registry descriptor.
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
    /* Behavior reads this word as a pointer, calls the pointer-to-member at its
       +0x08 on `this` every frame, and compares it against the file-scope state
       records data_ov062_0211e14c / _0211e15c / _0211e17c by ADDRESS.
       func_ov062_0211c658(this, record) is what assigns it. */
    void                        *mState;                /* 0x42c */
    /* PathPtr::GetNode writes the node at mPathNodeIndex here, and the
       carrying-a-star branch of InitResources then teleports the actor onto it
       (`mPosX = mPathNodePosX;` ...). */
    s32                          mPathNodePosX;         /* 0x430 */
    s32                          mPathNodePosY;         /* 0x434 */
    s32                          mPathNodePosZ;         /* 0x438 */
    u8  pad_43c[0x8];
    /* Counted down by DecIfAbove0_Short at the top of every frame, and reloaded
       with 0x1e the moment the carried actor is lost -- in the same breath as the
       state switch back to data_ov062_0211e17c. */
    s16                          mTimer;                /* 0x444 */
    u8  pad_446[0x2];
    u8                           unk_448;               /* 0x448 -- set to 2 when the spawn word's
                                                            item field reads 2 (which then forces
                                                            mCarriedItem to 1); read back as `!= 2`
                                                            twice, meaning unevidenced */
    u8  pad_449[0x1];
    s16                          unk_44a;               /* 0x44a -- Vec3_HorzAngle(&mPosX, &mSpawnPosX),
                                                            taken while the two are still equal;
                                                            no reader in the tree */
    s32                          mHeldActorID;          /* 0x44c */
    /* Behavior writes these three into the held actor's own mPosX/mPosY/mPosZ
       every frame -- this is where Klepto carries what it stole. */
    s32                          mHeldPosX;             /* 0x450 */
    s32                          mHeldPosY;             /* 0x454 */
    s32                          mHeldPosZ;             /* 0x458 */
    u8  pad_45c[0x8];
    s32                          mPathId;               /* 0x464 */
    s32                          mCarriedItem;          /* 0x468 */
    /* (param1 >> 12) & 0xf, used for one thing only: it is OR-ed with 0x50 to
       build the spawn parameter of the actor Klepto is carrying. */
    s32                          mHeldItemParam;        /* 0x46c */
    s32                          unk_470;               /* 0x470 -- InitResources stores 4; no reader */
    u32                          mPathNodeIndex;        /* 0x474 -- the index handed to PathPtr::GetNode */
    u8  pad_478[0xc];
    /* InitResources copies mPosX/mPosY/mPosZ here once, before any movement. */
    s32                          mSpawnPosX;            /* 0x484 */
    s32                          mSpawnPosY;            /* 0x488 */
    s32                          mSpawnPosZ;            /* 0x48c */

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
