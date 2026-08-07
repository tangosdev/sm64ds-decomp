/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ChiefChilly: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHIEFCHILLY_H
#define CHIEFCHILLY_H
#include "types.h"
#include "BlendModelAnim.h"

struct ChiefChilly {
    u8  pad_000[0x5c];
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
    u8  pad_0a4[0x6c];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* BlendModelAnim member, named by _ZN14BlendModelAnimD1Ev at +0x30c -- a relocation
       the ROM build checks. D1 and not D2, so it is this type and not an inlined base. The
       marker's pad stopped short of the object, so the member also takes over unk_368
       (+0x5c = speed), which the header declared separately inside it. */
    BlendModelAnim mBlendModelAnim;            /* 0x30c */
    u8  pad_37c[0x4];
    u8  mShadowModel;            /* 0x380 */
    u8  pad_381[0x57];
    s32 unk_3d8;            /* 0x3d8 */
    s32 unk_3dc;            /* 0x3dc */
    s32 unk_3e0;            /* 0x3e0 */
    u8  pad_3e4[0xd8];
    s32 unk_4bc;            /* 0x4bc */
    u8  pad_4c0[0x5];
    u8  unk_4c5;            /* 0x4c5 */
    u8  pad_4c6[0x5];
    u8  unk_4cb;            /* 0x4cb */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
