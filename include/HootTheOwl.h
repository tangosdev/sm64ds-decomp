#ifndef HOOTTHEOWL_H
#define HOOTTHEOWL_H

#include "types.h"

/* Derives from Enemy, and the class's own destructor is what proves it:
 * `_ZN10HootTheOwlD1Ev` stores this vtable, destroys the four members below in
 * reverse declaration order, then calls `Enemy::~Enemy`. Everything this header
 * used to restate below 0x110 belongs to that chain and is inherited now --
 * nine of those markers turned out to be named Actor/Enemy fields (mAngleX,
 * mPrevAngleX, mVertAccel, mTerminalVelocity, mVertSpeed, ...).
 *
 * THE FOUR MEMBERS CLOSE EXACTLY ON EACH OTHER, which is four independent
 * confirmations of one layout rather than four guesses:
 *
 *     0x110 MovingCylinderClsnWithPos  0x40   -> 0x150
 *     0x150 WithMeshClsn               0x1bc  -> 0x30c
 *     0x30c ModelAnim                  0x64   -> 0x370
 *     0x370 ShadowModel                0x28   -> 0x398
 *
 * and Enemy's own 0x110 closes exactly on the first of them.
 *
 * Typing them absorbed four markers that were their insides, each corroborated
 * by what the code does with it:
 *   - unk_128 = mMovingCylinderClsnWithPos.flags  (CylinderClsn +0x18)
 *   - mAnimation = the ModelAnim's Animation base (+0x50)
 *   - unk_364 = that Animation's currFrame (+0x08); Behavior reads it as
 *     `>> 12`, i.e. the integer frame of a 20.12 fixed-point count
 *   - unk_368 = that Animation's speed (+0x0c); Behavior copies mAnimSpeed
 *     into it, and InitResources sets mAnimSpeed to 0x1000, which is 1.0
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct HootTheOwl : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos; /* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    ShadowModel mShadowModel;         /* 0x370 */
    u8  pad_398[0x30];
    s32 mCurrentState;                /* 0x3c8 */
    s32 unk_3cc;                      /* 0x3cc */
    u8  pad_3d0[0x4];
    u8  unk_3d4;                      /* 0x3d4 */
    u8  pad_3d5[0xf];
    u8  unk_3e4;                      /* 0x3e4 */
    u8  pad_3e5[0xb];
    Fix12i mAnimSpeed;                /* 0x3f0 -- copied into mModelAnim.speed */

    /* --- vtable --- */
    virtual ~HootTheOwl();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char HootTheOwl_size_must_be_0x3f4[sizeof(HootTheOwl) == 0x3f4 ? 1 : -1];

#endif /* HOOTTHEOWL_H */
