/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Amp: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef AMP_H
#define AMP_H
#include "types.h"
#include "ModelAnim.h"
#include "Model.h"

struct Amp {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    /* 0x080..0x09c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_080;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
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
    u8  pad_0a4[0x30];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* Model member, named by _ZN5ModelD1Ev at +0x138 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x138 */
    u8  mTextureSequence;            /* 0x188 */
    u8  pad_189[0x13];
    u8  mTextureTransformer;            /* 0x19c */
    u8  pad_19d[0x13];
    u8  mShadowModel;            /* 0x1b0 */
    u8  pad_1b1[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x1d8 */
    u8  pad_1d9[0x3f];
    u8  mWithMeshClsn;            /* 0x218 */
    u8  pad_219[0x1bb];
    u8  unk_3d4;            /* 0x3d4 */
    u8  pad_3d5[0x4b];
    s32 unk_420;            /* 0x420 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
