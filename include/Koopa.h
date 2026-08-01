/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Koopa: 6 matched functions, 37 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KOOPA_H
#define KOOPA_H
#include "types.h"

struct Koopa {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0x2];
    u16 unk_104;            /* 0x104 */
    u8  unk_106;            /* 0x106 */
    u8  unk_107;            /* 0x107 */
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x3];
    s32 unk_10c;            /* 0x10c */
    u8  mCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mAnimation;            /* 0x350 */
    u8  pad_351[0x13];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x27];
    s32 mState;            /* 0x38c */
    s32 mKoopaVariant;            /* 0x390 */
    s32 mModelIndex;            /* 0x394 */
    u8  pad_398[0x4];
    s32 unk_39c;            /* 0x39c */
    s32 unk_3a0;            /* 0x3a0 */
    s32 unk_3a4;            /* 0x3a4 */
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
    s32 unk_3b0;            /* 0x3b0 */
    u8  pad_3b4[0x8];
    s32 unk_3bc;            /* 0x3bc */
    u8  pad_3c0[0x4];
    u16 unk_3c4;            /* 0x3c4 */
    u8  pad_3c6[0x4];
    u16 unk_3ca;            /* 0x3ca */
    u8  pad_3cc[0x2];
    u8  unk_3ce;            /* 0x3ce */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
