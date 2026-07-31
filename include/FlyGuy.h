/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FlyGuy: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLYGUY_H
#define FLYGUY_H
#include "types.h"

struct FlyGuy {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x38];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x54];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x63];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x5b];
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    s32 unk_3c8;            /* 0x3c8 */
    u8  pad_3cc[0x14];
    s32 unk_3e0;            /* 0x3e0 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
