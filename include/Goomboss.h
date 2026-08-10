#ifndef GOOMBOSS_H
#define GOOMBOSS_H

#include "types.h"

/* The Goomboss. Its destructor is the layout, and eight boundaries close on
 * sizes other headers assert:
 *
 *     Enemy                        ends 0x110
 *     MovingCylinderClsnWithPos[4] 0x110 + 4*0x40 = 0x210  -> ModelAnim
 *     ModelAnim                    0x210 +   0x64 = 0x274  -> the shadows
 *     ShadowModel[3]               0x274 + 3*0x28 = 0x2ec
 *     Vector3[3]                   0x3ac + 3*0x0c = 0x3d0  -> MaterialChanger
 *     MaterialChanger              0x3d0 +   0x14 = 0x3e4  -> TextureSequence
 *     TextureSequence              0x3e4 +   0x14 = 0x3f8  -> TextureTransformer
 *     TextureTransformer           0x3f8 +   0x14 = 0x40c  -> WithMeshClsn
 *     WithMeshClsn                 0x40c +  0x1bc = 0x5c8
 *
 * SIX OF THE GENERATED FIELDS WERE THE Vector3 ARRAY'S OWN COMPONENTS.
 * unk_3b8/3bc/3c0 are element 1's x/y/z and unk_3c4/3c8/3cc are element 2's --
 * the header had split a Vector3[3] into nine scalars and named six of them.
 * unk_3d8 and unk_3f0 are likewise inside the MaterialChanger and the
 * TextureSequence.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "TextureTransformer.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsnWithPos.h"

struct Goomboss : Enemy {
    MovingCylinderClsnWithPos mCylinderClsns[4];  /* 0x110 */
    ModelAnim mModelAnim;                         /* 0x210 */
    ShadowModel mShadowModels[3];                 /* 0x274 */
    u8  pad_2ec[0xc0];
    Vector3 unk_3ac[3];                           /* 0x3ac */
    MaterialChanger mMaterialChanger;             /* 0x3d0 */
    TextureSequence mTextureSequence;             /* 0x3e4 */
    TextureTransformer mTextureTransformer;       /* 0x3f8 */
    WithMeshClsn mWithMeshClsn;                   /* 0x40c */
    u8  pad_5c8[0x4];
    s32 unk_5cc;            /* 0x5cc */
    s32 unk_5d0;            /* 0x5d0 */
    s32 unk_5d4;            /* 0x5d4 */
    s32 unk_5d8;            /* 0x5d8 */
    s32 unk_5dc;            /* 0x5dc */
    u8  pad_5e0[0x4];
    s32 unk_5e4;            /* 0x5e4 */
    s32 unk_5e8;            /* 0x5e8 */
    u8  pad_5ec[0x4];
    s32 unk_5f0;            /* 0x5f0 */
    u16 unk_5f4;            /* 0x5f4 */
    s16 unk_5f6;            /* 0x5f6 */
    s16 unk_5f8;            /* 0x5f8 */
    u8  pad_5fa[0x4];
    u8  unk_5fe;            /* 0x5fe */
    u8  unk_5ff;            /* 0x5ff */
    u8  pad_600[0x4];
    u8  mSizeIndex;            /* 0x604 */
    u8  pad_605[0x5];
    u8  unk_60a;            /* 0x60a */

    virtual ~Goomboss();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 Render();
};

typedef char Goomboss_size_must_be_0x60c[sizeof(Goomboss) == 0x60c ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct Goomboss {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    /* 0x088..0x09c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x16c];
    u8  mModelAnim;            /* 0x210 */
    u8  pad_211[0x1a7];
    s32 unk_3b8;            /* 0x3b8 */
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    s32 unk_3c8;            /* 0x3c8 */
    s32 unk_3cc;            /* 0x3cc */
    u8  mMaterialChanger;            /* 0x3d0 */
    u8  pad_3d1[0x7];
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0x8];
    u8  mTextureSequence;            /* 0x3e4 */
    u8  pad_3e5[0xb];
    s32 unk_3f0;            /* 0x3f0 */
    u8  pad_3f4[0x4];
    u8  mTextureTransformer;            /* 0x3f8 */
    u8  pad_3f9[0x13];
    u8  mWithMeshClsn;            /* 0x40c */
    u8  pad_40d[0x1bf];
    s32 unk_5cc;            /* 0x5cc */
    s32 unk_5d0;            /* 0x5d0 */
    s32 unk_5d4;            /* 0x5d4 */
    s32 unk_5d8;            /* 0x5d8 */
    s32 unk_5dc;            /* 0x5dc */
    u8  pad_5e0[0x4];
    s32 unk_5e4;            /* 0x5e4 */
    s32 unk_5e8;            /* 0x5e8 */
    u8  pad_5ec[0x4];
    s32 unk_5f0;            /* 0x5f0 */
    u16 unk_5f4;            /* 0x5f4 */
    s16 unk_5f6;            /* 0x5f6 */
    s16 unk_5f8;            /* 0x5f8 */
    u8  pad_5fa[0x4];
    u8  unk_5fe;            /* 0x5fe */
    u8  unk_5ff;            /* 0x5ff */
    u8  pad_600[0x4];
    u8  mSizeIndex;            /* 0x604 */
    u8  pad_605[0x5];
    u8  unk_60a;            /* 0x60a */
};

#endif /* __cplusplus */

#endif /* GOOMBOSS_H */
