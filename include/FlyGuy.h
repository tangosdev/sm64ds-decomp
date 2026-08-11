#ifndef FLYGUY_H
#define FLYGUY_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN6FlyGuyD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 MovingCylinderClsn         0x34   -> 0x144
 *     0x144 WithMeshClsn               0x1bc  -> 0x300
 *     0x300 ModelAnim                  0x64   -> 0x364
 *     0x364 ShadowModel                0x28   -> 0x38c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `FlyGuy_Spawn` calls
 * `ActorBase::operator new(1000)` -- 0x3e8 -- and stores `_ZTV6FlyGuy`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3e4; the difference is trailing space no source reads.
 */

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "WithMeshClsn.h"

struct FlyGuy : Enemy {
    /* What mCurrentState points at. Behavior compares it against two objects in
       ov070's data and calls the handler at +0x08 through it. Both it and the
       timer at 0x3cc were reachable only as raw `c + 0x...`, so the generated
       header never had them. */
    struct State {
        u8  pad_00[0x8];
        void (FlyGuy::*mMain)();      /* 0x08 */
    };

    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    u8  pad_38c[0x30];
    State                       *mCurrentState;         /* 0x3bc */
    s32                          unk_3c0;               /* 0x3c0 */
    s32                          unk_3c4;               /* 0x3c4 */
    s32                          unk_3c8;               /* 0x3c8 */
    u16                          unk_3cc;               /* 0x3cc */
    u8  pad_3ce[0x12];
    s32                          unk_3e0;               /* 0x3e0 */
    u8  pad_3e4[0x4];

    /* --- vtable --- */
    virtual ~FlyGuy();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char FlyGuy_size_must_be_0x3e8[sizeof(FlyGuy) == 0x3e8 ? 1 : -1];

#endif /* FLYGUY_H */
