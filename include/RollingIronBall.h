#ifndef ROLLINGIRONBALL_H
#define ROLLINGIRONBALL_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN15RollingIronBallD1Ev` destroys each member, and
 * `RollingIronBall_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV15RollingIronBall`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 WithMeshClsn               0x1bc   -> 0x2cc
 *     0x2cc Model                      0x50    -> 0x31c
 *     0x31c ShadowModel                0x28    -> 0x344
 *     0x374 MovingCylinderClsn         0x34    -> 0x3a8
 *     0x3f4 PathPtr                    0x8     -> 0x3fc
 *
 * THE FIFTH MEMBER HAS NO DESTRUCTOR, so only the factory witnesses it:
 * RollingIronBall_Spawn constructs a PathPtr at 0x3f4 that the destructor
 * never destroys, PathPtr's being trivial. 0x3f4 + 8 is 0x3fc, exactly the
 * allocation literal -- the layout does not close without it. Reading only
 * the destructor leaves the class eight bytes short.
 *
 * (InitResources still does not reproduce -- a size disagreement, not a
 * compile error. That predates this header: it fails identically on 1b45f57b,
 * where the class was still flat. Its body carries laundering hacks and a
 * volatile read and wants its own matching session. Every other function of
 * this class reproduces.)
 *
 * SIZE IS THE ROM'S OWN: `RollingIronBall_Spawn` calls
 * `ActorBase::operator new(1020)` -- 0x3fc -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct RollingIronBall : Enemy {
    WithMeshClsn                 mWithMeshClsn;         /* 0x110 */
    Model                        mModel;                /* 0x2cc */
    ShadowModel                  mShadowModel;          /* 0x31c */
    u8  pad_344[0x30];
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x374 */
    s32                          unk_3a8;               /* 0x3a8 */
    s32                          unk_3ac;               /* 0x3ac */
    s32                          unk_3b0;               /* 0x3b0 */
    s32                          unk_3b4;               /* 0x3b4 */
    u8  pad_3b8[0x2];
    s16                          unk_3ba;               /* 0x3ba */
    u8  pad_3bc[0x4];
    s32                          unk_3c0;               /* 0x3c0 */
    s32                          unk_3c4;               /* 0x3c4 */
    s32                          unk_3c8;               /* 0x3c8 */
    s32                          unk_3cc;               /* 0x3cc */
    u8                           unk_3d0;               /* 0x3d0 */
    u8  pad_3d1[0x1];
    u8                           unk_3d2;               /* 0x3d2 */
    u8  pad_3d3[0x1];
    s32                          unk_3d4;               /* 0x3d4 */
    s32                          unk_3d8;               /* 0x3d8 */
    s32                          unk_3dc;               /* 0x3dc */
    s32                          unk_3e0;               /* 0x3e0 */
    s32                          unk_3e4;               /* 0x3e4 */
    u8  pad_3e8[0xc];
    PathPtr                      mPathPtr;              /* 0x3f4 */

    /* --- vtable --- */
    virtual ~RollingIronBall();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char RollingIronBall_size_must_be_0x3fc[sizeof(RollingIronBall) == 0x3fc ? 1 : -1];

#endif /* ROLLINGIRONBALL_H */
