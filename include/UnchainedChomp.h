/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class UnchainedChomp: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UNCHAINEDCHOMP_H
#define UNCHAINEDCHOMP_H
#include "types.h"
#include "ModelAnim.h"

struct UnchainedChomp {
    u8  pad_000[0x8];
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
    u8  pad_0a4[0x6c];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       ran 0x2d0 bytes PAST the end of the object; that space is not evidenced and stays
       explicit padding rather than being folded into the member. */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  pad_370[0x2d0];
    u8  mShadowModel;            /* 0x640 */
    u8  pad_641[0x6b];
    s32 unk_6ac;            /* 0x6ac */
    s32 unk_6b0;            /* 0x6b0 */
    s32 unk_6b4;            /* 0x6b4 */
    s32 unk_6b8;            /* 0x6b8 */
    u8  pad_6bc[0xd];
    u8  unk_6c9;            /* 0x6c9 */
    u8  pad_6ca[0x2];
    s32 unk_6cc;            /* 0x6cc */
    s32 unk_6d0;            /* 0x6d0 */
    s32 unk_6d4;            /* 0x6d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int Render();
#endif
};

#endif
