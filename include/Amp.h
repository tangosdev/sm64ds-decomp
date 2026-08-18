/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Amp: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef AMP_H
#define AMP_H
#include "types.h"
#include "ModelAnim.h"
#include "Model.h"
#include "TextureSequence.h"
#include "TextureTransformer.h"
#include "ShadowModel.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct Amp {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    /* 0x080..0x09c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
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
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x188 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN3AmpD0Ev.c] */
    TextureSequence mTextureSequence;            /* 0x188 */
    /* TextureTransformer member, named by the class's own destructor calling
       TextureTransformer's D1 at +0x19c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN3AmpD0Ev.c] */
    TextureTransformer mTextureTransformer;            /* 0x19c */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x1b0 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN3AmpD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x1b0 */
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x1d8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN3AmpD0Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x1d8 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x218 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN3AmpD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x218 */
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
