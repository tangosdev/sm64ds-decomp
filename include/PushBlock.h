/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PushBlock: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PUSHBLOCK_H
#define PUSHBLOCK_H
#include "types.h"

struct PushBlock {
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
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0xd];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mModel2;            /* 0x124 */
    u8  pad_125[0x4f];
    u8  mShadowModel;            /* 0x174 */
    u8  pad_175[0x57];
    u8  mMovingCylinderClsn;            /* 0x1cc */
    u8  pad_1cd[0x33];
    u8  mWithMeshClsn;            /* 0x200 */
    u8  pad_201[0x1bb];
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    u8  pad_3c4[0x6];
    u8  unk_3ca;            /* 0x3ca */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
