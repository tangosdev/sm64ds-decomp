/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SkiLift: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SKILIFT_H
#define SKILIFT_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct SkiLift {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0dc (+0x8 = data),
       mAnimation (+0x50 = the Animation base), which the header declared separately inside
       it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkiLiftD0Ev.c] */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x14c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkiLiftD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x14c */
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x174 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkiLiftD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x174 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x1a8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkiLiftD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x1a8 */
    s32 unk_364;            /* 0x364 */
    s32 unk_368;            /* 0x368 */
    s32 unk_36c;            /* 0x36c */
    u8  pad_370[0x4];
    s32 unk_374;            /* 0x374 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
