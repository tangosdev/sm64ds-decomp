/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Goomboss: 6 matched functions, 37 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef GOOMBOSS_H
#define GOOMBOSS_H
#include "types.h"

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
    u8  mScaleZ;            /* 0x088 */
    u8  pad_089[0x13];
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
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
