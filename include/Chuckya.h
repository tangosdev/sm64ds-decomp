#ifndef CHUCKYA_H
#define CHUCKYA_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN7ChuckyaD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another, and Enemy's own 0x110 closes
 * exactly on the first. Member NAMES are the ones this header already used --
 * a rebase should not also rename things its callers spell:
 *
 *     0x110 MovingCylinderClsn       0x34   -> 0x144
 *     0x144 WithMeshClsn             0x1bc  -> 0x300
 *     0x300 ModelAnim                0x64   -> 0x364
 *     0x368 ShadowModel              0x28   -> 0x390
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "BlendModelAnim.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct Chuckya : Enemy {
    MovingCylinderClsn           mCylinderClsn;         /* 0x110 */
    WithMeshClsn                 mMeshClsn;             /* 0x144 */
    ModelAnim                    mModel;                /* 0x300 */
    /* The state pointer, between the ModelAnim that ends at 0x364 and the
       ShadowModel that starts at 0x368. */
    void                        *mState;                /* 0x364 */
    ShadowModel                  mShadowModel;          /* 0x368 */
    u8  pad_390[0x30];
    s32                          mHomePosX;             /* 0x3c0 */
    s32                          mHomePosY;             /* 0x3c4 */
    s32                          mHomePosZ;             /* 0x3c8 */
    u8  pad_3cc[0xc];
    s32                          mPrevPosX;             /* 0x3d8 */
    s32                          mPrevPosY;             /* 0x3dc */
    s32                          mPrevPosZ;             /* 0x3e0 */
    u8                           unk_3e4;               /* 0x3e4 */
    u8  pad_3e5[0x1];
    u16                          unk_3e6;               /* 0x3e6 */
    u16                          unk_3e8;               /* 0x3e8 */
    u8  pad_3ea[0xe];
    void                        *mHeld;                 /* 0x3f8 */
    u8  pad_3fc[0x30];
    s32                          unk_42c;               /* 0x42c */
    s32                          unk_430;               /* 0x430 */
    s32                          unk_434;               /* 0x434 */

    /* --- vtable --- */
    virtual ~Chuckya();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Chuckya_size_must_be_0x438[sizeof(Chuckya) == 0x438 ? 1 : -1];

#endif /* CHUCKYA_H */
