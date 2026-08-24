/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class Bowser: 5 matched functions.
 *
 * Five sub-objects, and every one's asserted size closes EXACTLY on the next named
 * field -- five independent confirmations of one layout:
 *
 *     dActor_c                      0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     ModelAnim                  0x0d4 + 0x064 = 0x138   -> mTextureSequence
 *     TextureSequence            0x138 + 0x014 = 0x14c   -> mWithMeshClsn
 *     dBgCh_Actr               0x14c + 0x1bc = 0x308   -> mShadowModel
 *     ShadowModel                0x308 + 0x028 = 0x330   -> padding
 *     dCcAcPos_c  0x360 + 0x040 = 0x3a0   -> mTargetPlayer
 *
 * TWO OF THE GENERATED HEADER'S FIELDS WERE THE ModelAnim'S OWN INSIDES and are
 * gone from this half: `mAnimation` at 0x124 is 0x0d4 + 0x50, the Animation base
 * inside ModelAnim, and `unk_130` at 0x130 is 0x0d4 + 0x5c. Both were declared as
 * siblings of a `u8 mModelAnim` marker whose pad stopped short of the real object.
 * Same shape as Player's two ModelAnims.
 *
 * sizeof is 0x454, which is not inferred from the fields: Bowser_Spawn asks
 * fBase_c::operator new for 1108 bytes -- and the last declared field, unk_450,
 * happens to end there too.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSER_H
#define BOWSER_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct Bowser : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                                   /* 0x0d4 */
    TextureSequence mTextureSequence;                       /* 0x138 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x14c */
    ShadowModel mShadowModel;                               /* 0x308 */
    u8  pad_330[0x30];
    dCcAcPos_c mdCcAcPos_c;   /* 0x360 */
    /* A POINTER, not an s32. Bowser::Behavior assigns it straight from
       dActor_c::ClosestPlayer() and then re-spelt every read of the slot as
       `*(dActor_c **)((char *)&mTargetPlayer)`; typing it here deletes all three
       of those casts. Declared dActor_c* rather than Player* on purpose --
       Player.h is not includable here, and dActor_c is the base at offset 0, so
       the ClosestPlayer() result converts with no adjustment. */
    dActor_c *mTargetPlayer;      /* 0x3a0 */
    u8  pad_3a4[0x4];
    s32 unk_3a8;            /* 0x3a8 */
    u8  pad_3ac[0x4];
    s32 mHomePosX;            /* 0x3b0 */
    s32 mHomePosY;            /* 0x3b4 */
    s32 mHomePosZ;            /* 0x3b8 */
    u8  pad_3bc[0x30];
    s32 mDistToTarget;            /* 0x3ec */
    u8  pad_3f0[0x8];
    s32 mAnimSpeed;            /* 0x3f8 */
    s16 unk_3fc;            /* 0x3fc */
    u8  pad_3fe[0x8];
    s16 mAngleToTarget;            /* 0x406 */
    u8  pad_408[0x4];
    s32 unk_40c;            /* 0x40c */
    u8  pad_410[0x4];
    s8  unk_414;            /* 0x414 */
    u8  pad_415[0x1];
    s8  unk_416;            /* 0x416 */
    u8  pad_417[0x5];
    u8  unk_41c;            /* 0x41c */
    u8  pad_41d[0x6];
    s8  unk_423;            /* 0x423 */
    s8  unk_424;            /* 0x424 */
    u8  pad_425[0x1];
    s8  unk_426;            /* 0x426 */
    s8  unk_427;            /* 0x427 */
    u8  pad_428[0x1];
    s8  unk_429;            /* 0x429 */
    s8  unk_42a;            /* 0x42a */
    u8  unk_42b;            /* 0x42b */
    u8  pad_42c[0x18];
    s8  unk_444;            /* 0x444 */
    u8  pad_445[0x1];
    s8  unk_446;            /* 0x446 */
    u8  pad_447[0x1];
    s32 unk_448;            /* 0x448 */
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~Bowser();                  /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int InitResources();
    int CleanupResources();
    void OnPendingDestroy();
    int Behavior();
    int Render();
};

typedef char Bowser_size_must_be_0x454[sizeof(Bowser) == 0x454 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to inherit
   dActor_c's fields from and so spells the whole layout flat. Every current includer
   is a .cpp; this half is kept so that a future C one gets the right offsets
   rather than a parse error. */
struct Bowser {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x28];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x7];
    ModelAnim mModelAnim;                                   /* 0x0d4 */
    TextureSequence mTextureSequence;                       /* 0x138 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x14c */
    ShadowModel mShadowModel;                               /* 0x308 */
    u8  pad_330[0x30];
    dCcAcPos_c mdCcAcPos_c;   /* 0x360 */
    /* The C++ half types this dActor_c*; C translation units have no dActor_c
       declaration in scope here, so it is spelt void* -- same width, same slot. */
    void *mTargetPlayer;            /* 0x3a0 */
    u8  pad_3a4[0x4];
    s32 unk_3a8;            /* 0x3a8 */
    u8  pad_3ac[0x4];
    s32 mHomePosX;            /* 0x3b0 */
    s32 mHomePosY;            /* 0x3b4 */
    s32 mHomePosZ;            /* 0x3b8 */
    u8  pad_3bc[0x30];
    s32 mDistToTarget;            /* 0x3ec */
    u8  pad_3f0[0x8];
    s32 mAnimSpeed;            /* 0x3f8 */
    s16 unk_3fc;            /* 0x3fc */
    u8  pad_3fe[0x8];
    s16 mAngleToTarget;            /* 0x406 */
    u8  pad_408[0x4];
    s32 unk_40c;            /* 0x40c */
    u8  pad_410[0x4];
    s8  unk_414;            /* 0x414 */
    u8  pad_415[0x1];
    s8  unk_416;            /* 0x416 */
    u8  pad_417[0x5];
    u8  unk_41c;            /* 0x41c */
    u8  pad_41d[0x6];
    s8  unk_423;            /* 0x423 */
    s8  unk_424;            /* 0x424 */
    u8  pad_425[0x1];
    s8  unk_426;            /* 0x426 */
    s8  unk_427;            /* 0x427 */
    u8  pad_428[0x1];
    s8  unk_429;            /* 0x429 */
    s8  unk_42a;            /* 0x42a */
    u8  unk_42b;            /* 0x42b */
    u8  pad_42c[0x18];
    s8  unk_444;            /* 0x444 */
    u8  pad_445[0x1];
    s8  unk_446;            /* 0x446 */
    u8  pad_447[0x1];
    s32 unk_448;            /* 0x448 */
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
};

#endif /* __cplusplus */

#endif
