/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class QuestionBlock: 6 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef QUESTIONBLOCK_H
#define QUESTIONBLOCK_H
#include "types.h"

struct QuestionBlock {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorId;            /* 0x00c */
    u8  pad_00e[0x52];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x10];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mModelAnim;            /* 0x320 */
    u8  pad_321[0x4f];
    u8  mAnimation;            /* 0x370 */
    u8  pad_371[0x13];
    u8  mShadowModel;            /* 0x384 */
    u8  pad_385[0x27];
    u8  unk_3ac;            /* 0x3ac */
    u8  pad_3ad[0x33];
    s32 unk_3e0;            /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 unk_3e8;            /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8  unk_3f0;            /* 0x3f0 */
    u8  unk_3f1;            /* 0x3f1 */
    u8  pad_3f2[0x1];
    u8  unk_3f3;            /* 0x3f3 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
