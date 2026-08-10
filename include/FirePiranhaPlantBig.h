#ifndef FIREPIRANHAPLANTBIG_H
#define FIREPIRANHAPLANTBIG_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN19FirePiranhaPlantBigD1Ev`
 * stores this vtable, destroys three members, then calls `Enemy::~Enemy`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The members close exactly on one another, and Enemy's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 ModelAnim                  0x64   -> 0x174
 *     0x174 MovingCylinderClsn         0x34   -> 0x1a8
 *     0x1a8 MovingCylinderClsnWithPos  0x40   -> 0x1e8
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x160 mAnimation   = the Animation base of mModelAnim
 *   - 0x178 unk_178      = mMovingCylinderClsn.radius (+0x04)
 *   - 0x17c unk_17c      = mMovingCylinderClsn.height (+0x08)
 *   - 0x1ac unk_1ac      = mMovingCylinderClsnWithPos.radius (+0x04)
 *   - 0x1b0 unk_1b0      = mMovingCylinderClsnWithPos.height (+0x08)
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct FirePiranhaPlantBig : Enemy {
    ModelAnim                    mModelAnim;            /* 0x110 */
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x174 */
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x1a8 */
    s32                          unk_1e8;               /* 0x1e8 */
    s32                          mState;                /* 0x1ec */
    s32                          unk_1f0;               /* 0x1f0 */
    s32                          unk_1f4;               /* 0x1f4 */
    u8  pad_1f8[0xc];
    s32                          mScale;                /* 0x204 */
    s32                          unk_208;               /* 0x208 */
    s32                          unk_20c;               /* 0x20c */
    s32                          unk_210;               /* 0x210 */
    s32                          unk_214;               /* 0x214 */
    u8  pad_218[0x2];
    u8                           unk_21a;               /* 0x21a */
    u8                           unk_21b;               /* 0x21b */
    u8                           unk_21c;               /* 0x21c */
    u8                           unk_21d;               /* 0x21d */
    u8                           unk_21e;               /* 0x21e */
    u8                           unk_21f;               /* 0x21f */
    u8                           unk_220;               /* 0x220 */
    u8  pad_221[0x3];
    s32                          unk_224;               /* 0x224 */
    s32                          unk_228;               /* 0x228 */

    /* --- vtable --- */
    virtual ~FirePiranhaPlantBig();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FirePiranhaPlantBig_size_must_be_0x22c[sizeof(FirePiranhaPlantBig) == 0x22c ? 1 : -1];

#endif /* FIREPIRANHAPLANTBIG_H */
