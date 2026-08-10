#ifndef PIRANHAPLANT_H
#define PIRANHAPLANT_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN12PiranhaPlantD1Ev`
 * stores this vtable, destroys six members, then calls `Enemy::~Enemy`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The members close exactly on one another, and Enemy's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 ModelAnim                  0x64   -> 0x174
 *     0x174 Model                      0x50   -> 0x1c4
 *     0x1c4 WithMeshClsn               0x1bc  -> 0x380
 *     0x380 MovingCylinderClsn         0x34   -> 0x3b4
 *     0x3b4 MovingCylinderClsn         0x34   -> 0x3e8
 *     0x3e8 MovingCylinderClsnWithPos  0x40   -> 0x428
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x160 mAnimation   = the Animation base of mModelAnim
 *   - 0x170 unk_170      = mModelAnim.file (+0x60)
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

struct PiranhaPlant : Enemy {
    ModelAnim                    mModelAnim;            /* 0x110 */
    Model                        mModel;                /* 0x174 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x1c4 */
    MovingCylinderClsn           mMovingCylinderClsn1;  /* 0x380 */
    MovingCylinderClsn           mMovingCylinderClsn2;  /* 0x3b4 */
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x3e8 */
    u8                           unk_428;               /* 0x428 */
    u8  pad_429[0xb];
    s32                          unk_434;               /* 0x434 */
    s32                          unk_438;               /* 0x438 */
    s32                          unk_43c;               /* 0x43c */
    s32                          unk_440;               /* 0x440 */
    s32                          unk_444;               /* 0x444 */
    s32                          unk_448;               /* 0x448 */
    s32                          unk_44c;               /* 0x44c */
    s32                          unk_450;               /* 0x450 */
    s32                          unk_454;               /* 0x454 */
    s32                          mState;                /* 0x458 */
    u8                           unk_45c;               /* 0x45c */
    u8                           unk_45d;               /* 0x45d */
    u8  pad_45e[0x2];
    s32                          unk_460;               /* 0x460 */
    s32                          unk_464;               /* 0x464 */
    s16                          unk_468;               /* 0x468 */
    u8  pad_46a[0x2];
    s32                          unk_46c;               /* 0x46c */
    s32                          unk_470;               /* 0x470 */
    s32                          unk_474;               /* 0x474 */
    s32                          unk_478;               /* 0x478 */

    /* --- vtable --- */
    virtual ~PiranhaPlant();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char PiranhaPlant_size_must_be_0x47c[sizeof(PiranhaPlant) == 0x47c ? 1 : -1];

#endif /* PIRANHAPLANT_H */
