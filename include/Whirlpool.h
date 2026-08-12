#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN9SubmarineD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x114 ModelAnim                  0x64   -> 0x178
 *     0x178 TextureTransformer         0x14   -> 0x18c
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x184 unk_184      = mTextureTransformer + 0x0c
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

struct Submarine : Enemy {
    s32                          unk_110;               /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x114 */
    TextureTransformer           mTextureTransformer;   /* 0x178 */
    u8  pad_18c[0x1c];
    s32                          unk_1a8;               /* 0x1a8 */
    s32                          unk_1ac;               /* 0x1ac */
    s32                          unk_1b0;               /* 0x1b0 */
    u8  pad_1b4[0x4];
    u8                           unk_1b8;               /* 0x1b8 */
    u8  pad_1b9[0x3];

    /* --- vtable --- */
    virtual ~Submarine();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char Submarine_size_must_be_0x1bc[sizeof(Submarine) == 0x1bc ? 1 : -1];

#endif /* SUBMARINE_H */
