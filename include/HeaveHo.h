#ifndef HEAVEHO_H
#define HEAVEHO_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN7HeaveHoD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 MovingCylinderClsn         0x34   -> 0x144
 *     0x144 MovingCylinderClsnWithPos  0x40   -> 0x184
 *     0x184 WithMeshClsn               0x1bc  -> 0x340
 *     0x340 ModelAnim                  0x64   -> 0x3a4
 *     0x3a4 ShadowModel                0x28   -> 0x3cc
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x390 mAnimation   = mModelAnim + 0x50
 *   - 0x39c unk_39c      = mModelAnim + 0x5c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "WithMeshClsn.h"

struct HeaveHo : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x144 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x184 */
    ModelAnim                    mModelAnim;            /* 0x340 */
    ShadowModel                  mShadowModel;          /* 0x3a4 */
    u8  pad_3cc[0x30];
    s32                          unk_3fc;               /* 0x3fc */
    s32                          unk_400;               /* 0x400 */
    s32                          unk_404;               /* 0x404 */
    s32                          unk_408;               /* 0x408 */
    s32                          unk_40c;               /* 0x40c */
    s32                          unk_410;               /* 0x410 */
    s32                          unk_414;               /* 0x414 */
    s32                          unk_418;               /* 0x418 */
    u8  pad_41c[0xa];
    u8                           unk_426;               /* 0x426 */
    u8  pad_427[0x1];

    /* --- vtable --- */
    virtual ~HeaveHo();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char HeaveHo_size_must_be_0x428[sizeof(HeaveHo) == 0x428 ? 1 : -1];

#endif /* HEAVEHO_H */
